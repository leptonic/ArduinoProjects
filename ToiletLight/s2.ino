//  Author:Fred.Chu
//  Date:9 April,2013
//  catalex.taobao.com

//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//  Modified record:
//
/*******************************************************************************/
#include <TimerOne.h>
#include <avr/wdt.h>

#include <Wire.h>
#include "DS1307.h"
#include "TM1637.h"

#define RELAY

//
//#define DEBUG
//

#define ON 1
#define OFF 0
#define GREEN_LED 11
#define WHITE_LED 9
#define CLK 3//pins definitions for TM1637 and can be changed to other ports    
#define DIO 2

#define AFTERNOON_MODE 1
#define NIGHT_MODE     2
#define DAY_MODE     3
#define DELAYTIME 50
#define DIDA 10
#define IR_JUDGE_MAX 64
#define DEBOUNCE_TIME 500

DS1307 clock;//define a object of DS1307 class
int8_t TimeDisp[] = {0x00, 0x00, 0x00, 0x00};
unsigned char ClockPoint = 1;
int IrInputPin = 7;
int LightSensor = 8;
int sensorPin = A0;
int led_level = 0;
String comdata = "";
#ifdef RELAY
int relay_pin = 6;
#endif
//unsigned char Update=0;


TM1637 tm1637(CLK, DIO);

void setup()
{

  Serial.begin(9600);

  tm1637.set();
  tm1637.init();
  clock.begin();
  //  Timer1.initialize(1000000);//timing for 500ms
  // Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR
  pinMode(GREEN_LED, OUTPUT); //green led
  pinMode(WHITE_LED, OUTPUT); //white led
#ifdef RELAY
  pinMode(relay_pin, OUTPUT); //white led
#endif
  pinMode(IrInputPin, INPUT);
  pinMode(LightSensor, INPUT);
  wdt_enable(WDTO_1S);
  relay_sw(false);

  Serial.print("AT+CIPMUX=1\r\n");
  delays(500);
  Serial.print("AT+CIPSERVER=1\r\n");
}
bool is_wifitime()
{
  clock.getTime();
  if ((clock.hour == 21) && (clock.minute < 59) && (clock.minute > 30))
  {
    return 1;
  }
  else
  {
    return 0;
  }

}
void reconnect_wifi()
{
  Serial.print("AT+CIPMUX=1\r\n");
  delays(500);
  Serial.print("AT+CIPSERVER=1\r\n");
}
void receive_wifi()
{

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
    mh = comdata.indexOf(':');
    if ((comdata[mh + 1] == 'T') && (comdata[mh + 2] == 'T'))
    {
      festival_led();
    }
    else if (comdata[mh + 1] == 'S')
    {
      int year, month, day, hour, minter;
      int dayweek;
      String temp;
      clock.getTime();
      month = clock.month;
      day = clock.dayOfMonth;
      year = clock.year + 2000;
      dayweek = clock.dayOfWeek;
      delay(100);
      clock.fillByYMD(year, month, day); //Jan 19,2013
      clock.fillDayOfWeek(dayweek);//Saturday


      temp.remove(0);
      temp += comdata[mh + 2];
      temp += comdata[mh + 3];
      hour = temp.toInt();
      temp.remove(0);
      temp += comdata[mh + 4];
      temp += comdata[mh + 5];
      minter = temp.toInt();
      temp.remove(0);
      if ((hour < 24) && (minter < 60))
      {
        clock.fillByHMS(hour, minter, 00); //15:28 30"
        clock.setTime();
        festival_led();
      }
      else//error
      {
        error_led();
      }
    }


  }


}

void delays(unsigned long ms)
{
  uint16_t start = (uint16_t)micros();
  if (is_wifitime())
    receive_wifi();
  while (ms > 0)
  {
    if (((uint16_t)micros() - start) >= 1000)
    {
      ms--;
      start += 900;
    }
    wdt_reset();
  }
}
#ifdef RELAY
void relay_sw(bool sw)
{
  if (sw == true)
  {
    delays(1700);
    if (human_judge())
    {
      sw = !sw;
      digitalWrite(relay_pin, sw);
      delays(1000);
      led_level = 0;
      analogWrite(WHITE_LED, 0);
    }
    else
    {
      return;
    }
  }
  else
  {
    sw = !sw;
    digitalWrite(relay_pin, sw);
  }

}
#endif
void ramp_up()
{
  int ilevel;
  for (ilevel = led_level; ilevel < 256; ilevel++)
  {
    if (ilevel == 0)
      ilevel = 50;

    analogWrite(WHITE_LED, ilevel);
    delays(12);
  }
#ifdef RELAY
  relay_sw(true);

#endif
  led_level = ilevel;
}
void ramp_up_night()
{
  int ilevel;
  if (led_level == 0)
  {
    for (ilevel = 0; ilevel < 150; ilevel++)
    {
      // wdt_reset();
      if (ilevel < 50)
      {
        analogWrite(WHITE_LED, ilevel);
        delays(30);

      }
      else if ((ilevel < 120) && (ilevel > 50))
      {
        analogWrite(WHITE_LED, ilevel);
        delays(40);
      }
      else
      {
        analogWrite(WHITE_LED, ilevel);
        delays(DELAYTIME);
      }
    }
    for (ilevel =  150; ilevel > 120; ilevel--)
    {
      // wdt_reset();
      analogWrite(WHITE_LED, ilevel);
      delays(20);
    }
    for (ilevel = 120; ilevel < 220; ilevel++)
    {
      if ((ilevel < 120) && (ilevel > 50))
      {
        analogWrite(WHITE_LED, ilevel);
        delays(DELAYTIME);
      }
      else
      {
        analogWrite(WHITE_LED, ilevel);
        delays(DELAYTIME * 2);
      }
    }
  }
  else // ilevel!=0
  {
    for (ilevel = led_level; ilevel < 200; ilevel++)
    {
      // wdt_reset();
      if (ilevel < 50)
      {
        analogWrite(WHITE_LED, ilevel);
        delays(20);

      }
      else if ((ilevel < 120) && (ilevel > 50))
      {
        analogWrite(WHITE_LED, ilevel);
        delays(DELAYTIME);
      }
      else
      {
        analogWrite(WHITE_LED, ilevel);
        delays(DELAYTIME * 2);
      }
    }
  }
  led_level = ilevel;
}

void ramp_up_day()
{
  int ilevel;

  delays(500);
  analogWrite(WHITE_LED, 255);
  led_level = 0;
}

int check_again_IR()
{
  bool value[IR_JUDGE_MAX];
  int i;

  for (i = 0; i < IR_JUDGE_MAX; i++)
  {
    value[i] = digitalRead(IrInputPin);
    if (value[i] == 1)
      return 1;
    delays(500);
  }


  return 0;


}
bool is_festival()
{
  if ((clock.month == 1) && (clock.dayOfMonth == 23))
  {
    if ((clock.hour == 9) || (clock.hour == 8) || (clock.hour == 12) || (clock.hour == 20) || (clock.hour == 21) || (clock.hour == 22))
      return 1;
  }
  if ((clock.month == 10) && (clock.dayOfMonth == 13))
  {
    if ((clock.hour == 9) || (clock.hour == 8) || (clock.hour == 12) || (clock.hour == 20) || (clock.hour == 21) || (clock.hour == 22))
      return 1;
  }
  if ((clock.month == 2) && (clock.dayOfMonth == 2))
  {
    if ((clock.hour == 9) || (clock.hour == 8) || (clock.hour == 12) || (clock.hour == 20) || (clock.hour == 21) || (clock.hour == 22))
      return 1;
  }
  if ((clock.month == 8) && (clock.dayOfMonth == 16))
  {
    if ((clock.hour == 9) || (clock.hour == 8) || (clock.hour == 12) || (clock.hour == 20) || (clock.hour == 21) || (clock.hour == 22))
      return 1;
  }
  if ((clock.month == 6) && (clock.dayOfMonth == 13))
  {
    if ((clock.hour == 9) || (clock.hour == 8) || (clock.hour == 12) || (clock.hour == 20) || (clock.hour == 21) || (clock.hour == 22))
      return 1;
  }

  return 0;
}
void error_led()
{
  analogWrite(GREEN_LED, 255);
  delay(200);
  analogWrite(GREEN_LED, 0);
  delay(750);
  analogWrite(GREEN_LED, 255);
  delay(750);
  analogWrite(GREEN_LED, 0);
  delay(300);
  analogWrite(GREEN_LED, 255);

}
void festival_led()
{
  analogWrite(GREEN_LED, 255);
  delay(2200);
  analogWrite(GREEN_LED, 0);



}
bool daymode_judge_lsensor()//return 1 means don't need open
{
  //return (!digitalRead(LightSensor));
  bool sensorValue = 0;
  bool sensorValue2 = 0;

  sensorValue = digitalRead(LightSensor);
  delay(DEBOUNCE_TIME);
  sensorValue2 = digitalRead(LightSensor);
  delay(10);
  if (sensorValue == sensorValue2)
  {
    return sensorValue;

  }
  else
  {
    sensorValue = daymode_judge_lsensor();
    return sensorValue;
  }





#ifdef DEBUG
  //char s2[8];
  s2[0] = 'G';
  s2[1] = '=';
  sprintf(s2 + 2, "%d ", sensorValue);
  Serial.println(s2);
#endif

  /*
    sensorValue = analogRead(sensorPin);
    delays(500);
    if (sensorValue > 150)
    {
      return 0; //need open
    }
    else
    {
      return 1;
    }
    */
}
bool daymode_judge_day() //return 1 means don't need open
{
  switch (clock.month)
  {
    case 1:
    case 2:
    case 3:
      if ((clock.hour > 7) && (clock.hour < 17))
        return 1;
      else
        return 0;
      break;
    case 4:
      if ((clock.hour > 7) && (clock.hour < 18))
        return 1;
      else
        return 0;
      break;
    case 5:
    case 6:
    case 7:
      if ((clock.hour > 6) && (clock.hour < 19))
        return 1;
      else
        return 0;
      break;
    case 8:
      if ((clock.hour > 6) && (clock.hour < 18))
        return 1;
      else
        return 0;
      break;
    case 9:
      if ((clock.hour > 6) && (clock.hour < 17))
        return 1;
      else
        return 0;
      break;
    case 10:
    case 11:
    case 12:
      if ((clock.hour > 7) && (clock.hour < 17))
        return 1;
      else
        return 0;
      break;
    default:
      break;
  }

}

int human_judge()
{
  int v1, v2;
  v1 = 0;
  v2 = 0;
  v1 = digitalRead(IrInputPin);
  delays(DEBOUNCE_TIME);
  v2 = digitalRead(IrInputPin);
  delays(10);
  if (v1 == v2)
    return v1;
  else
  {
    v1 = human_judge();
    return v1;
  }

}
void led_IR_control()
{
  int value = 0;
  clock.getTime();
  if (is_wifitime())
  {
    if (clock.second == 15)
    {
      reconnect_wifi();
    }
  }
  if (clock.hour > 25)
  {
    led_IR_control();
  }
  if (daymode_judge_day())// is day
  {
    ClockPoint = (~ClockPoint) & 0x01;
    time_display();
    if ((daymode_judge_lsensor()) && (human_judge())) // is darken and human coming
    {
      analogWrite(WHITE_LED, 180);
      delays(500);
      analogWrite(WHITE_LED, 255);
      delays(1500);
#ifdef RELAY
      if (human_judge())
        relay_sw(true);

#endif
      delays(60000);

    }
    else
    {
      delays(1000);
     if (!human_judge())
     {
#ifdef RELAY
      relay_sw(false);
#endif
      led_level = 0;
      analogWrite(WHITE_LED, 0);
     }
    }

  }
  else
  {
    ClockPoint = 0x01;
    time_display();
    value = human_judge();

    if (value == 1)
    {
      if (is_festival())
      {
        festival_led();
      }

      if ((clock.hour > 22) || (clock.hour < 7))
      { //night mode

#ifdef RELAY

        relay_sw(false);

#endif
        ramp_up_night();

      }
      else //afternoon mode
      {
        ramp_up();

      }
      delays(60000);
    }
    else
    {
      if ( !check_again_IR())
      {
        led_level = 0;
        analogWrite(WHITE_LED, 0);
        analogWrite(GREEN_LED, 0);
#ifdef RELAY

        relay_sw(false);

#endif
        tm1637.clearDisplay();
      }

    }
  }

}

void loop()
{




#ifndef DEBUG
  led_IR_control();

#else


  daymode_judge();
#endif

  wdt_reset();
}


void time_display()
{

  TimeUpdate();
  tm1637.display(TimeDisp);

}

void TimeUpdate(void)
{
  clock.getTime();
  delay(100);
  if (ClockPoint)tm1637.point(POINT_ON);
  else tm1637.point(POINT_OFF);
  if (clock.hour < 25)
  {
    TimeDisp[0] = clock.hour / 10;
    TimeDisp[1] = clock.hour % 10;
    TimeDisp[2] = clock.minute / 10;
    TimeDisp[3] = clock.minute % 10;
  }
  //Update = OFF;
}


