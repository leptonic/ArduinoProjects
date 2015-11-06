#include <Adafruit_NeoPixel.h>
#include <avr/wdt.h>
#define PIN_LIGHT 12
#define PIN_LAZER 7
#define PIN_LAZER_RECEIVE A0
#define PIN_RELAY 9

#define DEBUG
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN_LIGHT, NEO_GRB + NEO_KHZ800);
String comdata = "";
String Phone1_MAC="00:f7:6f:7c:3c:2d";
String Phone2_MAC="64:9a:be:cc:89:b4";
//Initiztion
void setup() {
  //watch dog
    wdt_enable(WDTO_8S);
  // lazer part
  Serial.begin(9600);
  pinMode(PIN_LAZER, OUTPUT);  //   定义13脚为数字输出接口
  //analogReference(INTERNAL);
  //lazer part over

  //light part
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  //light over


}
//*************************
//LAZER PART
bool check_lazer()
{

  double result;
  wdt_reset();
  // put your main code here, to run repeatedly:
  digitalWrite(PIN_LAZER, HIGH);   // 打开激光头
  delay(50);
  result = analogRead(PIN_LAZER_RECEIVE);
  #ifdef DEBUG
  Serial.print("result=");
  Serial.println(result);
  #endif
  delay(200);              // 延时一秒
  digitalWrite(PIN_LAZER, LOW);    // 关闭激光头
//  delay(500);              // 延时一秒
  if (result < 300)// lazer was cutted
  {
    return true;
  }
  else
  {
    return false;
  }
}
//*************************
//LIGHT PART
// base
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
//effect
void wifi_verify_ok()
{
  strip.setBrightness(100);
  colorWipe(strip.Color(0, 0, 255), 50); // blue
  colorWipe(strip.Color(0, 255, 0), 50); // Green

}
void ramp_up()
{
  strip.setBrightness(255);
  colorWipe(strip.Color(255, 255, 255), 100); // White

}
void ramp_down()
{
  colorWipe(strip.Color(0, 0, 0), 300); 

}
void open_door_light()
{
 strip.setPixelColor(strip.Color(0, 255, 0), 3); 
  strip.show();
     delay(200);
 strip.setPixelColor(strip.Color(0, 0, 0), 3); 
  strip.show();
  delay(200);
   strip.setPixelColor(strip.Color(0, 255, 0), 3); 
  strip.show();
     delay(200);
 strip.setPixelColor(strip.Color(0, 0, 0), 3); 
  strip.show();
  delay(200);
   strip.setPixelColor(strip.Color(0, 255, 0), 3); 
  strip.show();
     delay(200);
 strip.setPixelColor(strip.Color(0, 0, 0), 3); 
  strip.show();
 
}


//*************************
// WIFI PART
void wifi_set_deafult()
{
  wdt_reset();
  Serial.print("AT+CWMODE=2\r\n");
  delay(2000);
  Serial.print("AT+CWSAP=\"DBv0.1\",\"phoenixislarus\",2,2\r\n");
    wdt_reset();
  delay(5000);
    wdt_reset();
  Serial.print("AT+RST\r\n");
  delay(2000);
}
void wifi_scan()
{
  Serial.print("AT+CWLIF\r\n");
  
}
void wifi_reset()
{
 Serial.print("AT+RST\r\n");
   wdt_reset();
 delay(2000);
}
bool receive_wifi()
{
  wdt_reset();
  comdata.remove(0);
  while (Serial.available() > 0)
  {
    comdata += char(Serial.read());
    delay(2);
  }
  if (comdata.length() > 9)
  {
    //Serial.println(comdata);
    int mh;
    int getmac;
    mh = comdata.indexOf(':');
    if(mh>0) //found it
    {
      //00:f7:6f:7c:3c:2d Leon iPhone5s number
     if((comdata.substring((mh-2),(mh-2+17))==Phone1_MAC)||(comdata.substring((mh-2),(mh-2+17))==Phone2_MAC))
     {
       wifi_verify_ok();
       delay(20);
       ramp_down();
       return true;
     }
    }
  }
  return false;
}
//*************************
//REPLY PART
void open_door()
{
  //open
digitalWrite(PIN_RELAY,HIGH); // open relay
  //delay
    wdt_reset();
  delay(2000);
digitalWrite(PIN_RELAY,LOW); // close relay
  //close


}

//*************************
//*=====================
//MAIN PART
#ifdef DEBUG
// DEBUG mode
void loop(){
  bool lazer_check_result;
    lazer_check_result=check_lazer();
    Serial.print("lazer_check");
    Serial.println(lazer_check_result);
    
    
  
}
#else // running mode
void loop() {
bool lazer_check_result;
bool wifi_check_result;

wifi_scan();

wifi_check_result=receive_wifi();
  lazer_check_result=check_lazer();

// only use key open door
  if((lazer_check_result)&&(!wifi_check_result))
  {

    ramp_up();
    delay(1000);
     wdt_reset();
     delay(1000);
     wdt_reset();
     delay(1000);
     wdt_reset();
     delay(1000);
     wdt_reset();
    ramp_down();
  }
 else if((lazer_check_result)&&(wifi_check_result)) // Master is coming... 
 {
   open_door_light();
   open_door();
   
    ramp_up();
    delay(1000);
     wdt_reset();
     delay(1000);
     wdt_reset();
     delay(1000);
     wdt_reset();
     delay(1000);
     wdt_reset();
    ramp_down();
 
 }
   
   
   
   

  delay(500);

}
#endif
