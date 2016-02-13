/*
Modified by Lauren
 version 0.2

 Any suggestions are welcome.
 E-mail: Lauran.pan@gmail.com
 */

#include "LCD4884.h"
#include "DFrobot_bmp.h"
#include "DFrobot_chinese.h"
#include <Wire.h>
#include "DS1307.h"

#include <SPI.h>
//#include <SD.h>
//File myFile;

#include <SFE_BMP180.h>
SFE_BMP180 pressure;
#define ALTITUDE 1655.0

double baseline; // baseline pressure

DS1307 clock;//define a object of DS1307 class
//keypad debounce parameter
#define DEBOUNCE_MAX 15
#define DEBOUNCE_ON  10
#define DEBOUNCE_OFF 3

#define NUM_KEYS 5

#define NUM_MENU_ITEM	4

// joystick number
#define LEFT_KEY 0
#define CENTER_KEY 1
#define DOWN_KEY 2
#define RIGHT_KEY 3
#define UP_KEY 4

// menu starting points

#define MENU_X	10		// 0-83
#define MENU_Y	1		// 0-5


int  adc_key_val[5] = {
  50, 200, 400, 600, 800
};
char te[8];
char pe[8];
// debounce counters
byte button_count[NUM_KEYS];
// button status - pressed/released
byte button_status[NUM_KEYS];
// button on flags for user program
byte button_flag[NUM_KEYS];
bool inprocess;
// menu definition
char menu_items[NUM_MENU_ITEM][12] = {
  "TIME",
  "ENVIRONMENT",
  "RUN",
  "ABOUT"
};

void (*menu_funcs[NUM_MENU_ITEM])(void) = {
  time,
  env,
  runs,
  about
};

char current_menu_item;
char s[32];

void setup()
{
  Serial.begin(9600);
  //bmp180
//  Serial.println("REBOOT");

  // Initialize the sensor (it is important to get calibration values stored on the device).

  if (pressure.begin())
  {
  }
  //  Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

  //  Serial.println("BMP180 init fail (disconnected?)\n\n");
    while (1); // Pause forever.
  }

  ///sdcard v
  #if 0
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


 // Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(10, OUTPUT);

  if (!SD.begin(8)) {
  //  Serial.println("initialization failed!");
    return;
  }
  //Serial.println("initialization done.");


#endif

  ////clock v
  clock.begin();
  //settime();
  inprocess = false;
  /////display v

  // setup interrupt-driven keypad arrays
  // reset button arrays
  for (byte i = 0; i < NUM_KEYS; i++) {
    button_count[i] = 0;
    button_status[i] = 0;
    button_flag[i] = 0;
  }

  // Setup timer2 -- Prescaler/256
  TCCR2A &= ~((1 << WGM21) | (1 << WGM20));
  TCCR2B &= ~(1 << WGM22);
  TCCR2B = (1 << CS22) | (1 << CS21);

  ASSR |= (0 << AS2);

  // Use normal mode
  TCCR2A = 0;
  //Timer2 Overflow Interrupt Enable
  TIMSK2 |= (0 << OCIE2A);
  TCNT2 = 0x6; // counting starts from 6;
  TIMSK2 = (1 << TOIE2);



  SREG |= 1 << SREG_I;

  lcd.LCD_init();
  lcd.LCD_clear();

  //menu initialization
  init_MENU();
  current_menu_item = 0;

  lcd.backlight(ON);//Turn on the backlight
  //lcd.backlight(OFF); // Turn off the backlight
}
void settime()
{
  clock.fillByYMD(2016, 1, 26); //Jan 19,2013
  clock.fillByHMS(23, 15, 30); //15:28 30"
  clock.fillDayOfWeek(TUE);//Saturday
  clock.setTime();//write time to the RTC chip
}




void printTime()
{

}
/* loop */

void loop()
{
  //printTime();
#if 1
  byte i;
  for (i = 0; i < NUM_KEYS; i++) {
    if (button_flag[i] != 0) {

      button_flag[i] = 0; // reset button flag
      switch (i) {

        case UP_KEY:
          // current item to normal display
          lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_NORMAL );
          current_menu_item -= 1;
          if (current_menu_item < 0)  current_menu_item = NUM_MENU_ITEM - 1;
          // next item to highlight display
          lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_HIGHLIGHT );
          break;
        case DOWN_KEY:
          // current item to normal display
          lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_NORMAL );
          current_menu_item += 1;
          if (current_menu_item > (NUM_MENU_ITEM - 1))  current_menu_item = 0;
          // next item to highlight display
          lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_HIGHLIGHT );
          break;
        case LEFT_KEY:
          init_MENU();
          current_menu_item = 0;
          break;
        case CENTER_KEY:
          lcd.LCD_clear();
          (*menu_funcs[current_menu_item])();
          lcd.LCD_clear();
          init_MENU();
          current_menu_item = 0;
          break;
      }

    }
  }
#endif
}

/* menu functions */

void init_MENU(void) {

  byte i;

  lcd.LCD_clear();

  lcd.LCD_write_string(MENU_X, MENU_Y, menu_items[0], MENU_HIGHLIGHT );

  for (i = 1; i < NUM_MENU_ITEM; i++) {
    lcd.LCD_write_string(MENU_X, MENU_Y + i, menu_items[i], MENU_NORMAL);
  }


}

// waiting for center key press
void waitfor_OKkey() {
  byte i;
  byte key = 0xFF;
  while (key != CENTER_KEY) {
    for (i = 0; i < NUM_KEYS; i++) {
      if (button_flag[i] != 0) {
        button_flag[i] = 0; // reset button flag
        if (i == CENTER_KEY) key = CENTER_KEY;
      }
    }
  }

}

void time()
{
  inprocess = true;
  while (inprocess)
  {
    clock.getTime();

    sprintf(s, "%4d-%2d-%2d     %2d:%2d'%2d", clock.year + 2000, clock.month, clock.dayOfMonth, clock.hour, clock.minute , clock.second);


  //  Serial.println(s);


    //  lcd.LCD_write_string_big(10, 1, s, MENU_NORMAL);
    lcd.LCD_write_string(10, 1, s, MENU_NORMAL);
    // lcd.LCD_write_string(78, 2, "C", MENU_NORMAL);
    lcd.LCD_write_string(38, 5, "OK", MENU_HIGHLIGHT );

  }
  waitfor_OKkey();

}
void env()
{
  char status;
  double T, P, p0, a;
  double Tr;
  char *s;
  s = (char *)malloc(sizeof(char) * 30);
  ///display
  inprocess = true;
  while (inprocess)
  {
    status = pressure.startTemperature();
    if (status != 0)
    {
      // Wait for the measurement to complete:
      delay(status);


      status = pressure.getTemperature(T);
      if (status != 0)
      {
        Tr = T;
        status = pressure.startPressure(3);
        if (status != 0)
        {
          // Wait for the measurement to complete:
          delay(status);

          status = pressure.getPressure(P, T);
          if (status != 0)
          {
        //    Serial.print("te: ");
       //     Serial.print(Tr, 2);

      //      Serial.print(" deg C, ");
      //      Serial.print("ap: ");
       //     Serial.print(P, 2);
       //     Serial.print(" mb, ");
            // sprintf(s,"%f mb     %8f c", P,Tr);


            int uv;
            dtostrf(P, 4, 2, pe);  // 相當於 %6.3f
            dtostrf(Tr, 4, 2, te);  // 相當於 %6.3f
            uv = analogRead(A3);
            sprintf(s, "%s mb    %s c      %d", pe, te, uv);
   //         Serial.print(s);
            // The pressure sensor returns abolute pressure, which varies with altitude.
            // To remove the effects of altitude, use the sealevel function and your current altitude.
            // This number is commonly used in weather reports.
            // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
            // Result: p0 = sea-level compensated pressure in mb

            // On the other hand, if you want to determine your altitude from the pressure reading,
            // use the altitude function along with a baseline pressure (sea-level or other).
            // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
            // Result: a = altitude in m.


          }
        //  else Serial.println("error retrieving pressure measurement\n");
        }
   //     else Serial.println("error starting pressure measurement\n");
      }
  //    else Serial.println("error retrieving temperature measurement\n");
    }
 //   else Serial.println("error starting temperature measurement\n");

    delay(3000);  // Pause for 5 seconds.




    lcd.LCD_write_string(10, 1, s, MENU_NORMAL);

    lcd.LCD_write_string(38, 5, "OK", MENU_HIGHLIGHT );
    free(s);

  }
  waitfor_OKkey();
  delay(500);
}

void charmap() {
  char i, j;
  for (i = 0; i < 5; i++) {
    for (j = 0; j < 14; j++) {
      lcd.LCD_set_XY(j * 6, i);
      lcd.LCD_write_char(i * 14 + j + 32, MENU_NORMAL);
    }
  }


  lcd.LCD_write_string(38, 5, "OK", MENU_HIGHLIGHT );
  waitfor_OKkey();
}

void runs()
{
  writelog();
}


void about() {

  lcd.LCD_write_string( 0, 1, "Weather Station", MENU_NORMAL);
  lcd.LCD_write_string( 0, 3, "leptonic", MENU_NORMAL);
  lcd.LCD_write_string(38, 5, "OK", MENU_HIGHLIGHT );
  waitfor_OKkey();


}



// The followinging are interrupt-driven keypad reading functions
// which includes DEBOUNCE ON/OFF mechanism, and continuous pressing detection


// Convert ADC value to key number
char get_key(unsigned int input)
{
  char k;

  for (k = 0; k < NUM_KEYS; k++)
  {
    if (input < adc_key_val[k])
    {

      return k;
    }
  }

  if (k >= NUM_KEYS)
    k = -1;     // No valid key pressed

  return k;
}

void update_adc_key() {
  int adc_key_in;
  char key_in;
  byte i;

  adc_key_in = analogRead(0);
  key_in = get_key(adc_key_in);
  for (i = 0; i < NUM_KEYS; i++)
  {
    if (key_in == i) //one key is pressed
    {

      if (button_count[i] < DEBOUNCE_MAX)
      {
        button_count[i]++;
        if (button_count[i] > DEBOUNCE_ON)
        {
          if (button_status[i] == 0)
          {
            button_flag[i] = 1;
            button_status[i] = 1; //button debounced to 'pressed' status
            inprocess = false;
          }

        }
      }

    }
    else // no button pressed
    {
      if (button_count[i] > 0)
      {
        button_flag[i] = 0;
        button_count[i]--;
        if (button_count[i] < DEBOUNCE_OFF) {
          button_status[i] = 0; //button debounced to 'released' status
        }
      }
    }

  }
}

// Timer2 interrupt routine -
// 1/(160000000/256/(256-6)) = 4ms interval

ISR(TIMER2_OVF_vect) {
  TCNT2  = 6;
  update_adc_key();
}

void writelog()
{
  // Serial.println("Creating example.txt...");
  // myFile = SD.open("example.csv", FILE_WRITE);
  // myFile.println("a,b,c,d");
  // myFile.println("1,2,3,4");
  // myFile.close();



  inprocess = true;
  double T, P;
  int uv;
  char status;
  //Serial.println("0");
  while (inprocess)
  {
    clock.getTime();
    
        status = pressure.startTemperature();

        if (status != 0)
        {
          delay(100);

          status = pressure.getTemperature(T);
        while(status==0)
        {

         status = pressure.getTemperature(T);
         delay(1000);

        }
        }
    status = pressure.startPressure(3);


    if (status != 0)
    {

      delay(100);
      status = pressure.getPressure(P, T);
      while (status == 0)
      {

        status = pressure.getPressure(P, T);
        delay(1000);

      }
      dtostrf(T, 4, 2, te);  // 相當於 %6.3f
      dtostrf(P, 4, 2, pe);  // 相當於 %6.3f

      //  char *s;
      //s = (char *)malloc(sizeof(char) * 21);
      uv = analogRead(A3);
      delay(1000);
      sprintf(s, "%d%-d-%d,%d:%d,%s,%s,%d", clock.year + 2000, clock.month, clock.dayOfMonth, clock.hour, clock.minute, te, pe, uv);
     // myFile = SD.open("logs.csv", FILE_WRITE);

     
   //   myFile.println(s);

    
      Serial.println(s);
       lcd.LCD_write_string(10, 1, s, MENU_NORMAL);
    
   //   myFile.close();

    delay(10000);

    }

    delay(2000);



  

}
//Serial.println("4");
}
double getPressure()
{
  char status;
  double T, P, p0, a;

  // You must first get a temperature measurement to perform a pressure reading.

  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:

    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          return (P);
        }
    //    else Serial.println("error retrieving pressure measurement\n");
      }
   //   else Serial.println("error starting pressure measurement\n");
    }
   // else Serial.println("error retrieving temperature measurement\n");
  }
//  else Serial.println("error starting temperature measurement\n");
}



