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

#define ON 1
#define OFF 0

#define CLK 3//pins definitions for TM1637 and can be changed to other ports    
#define DIO 2
#define RELAYPIN 4

//
//#define DEBUG
//
#define IR_JUDGE_MAX 64

DS1307 clock;//define a object of DS1307 class
int8_t TimeDisp[] = {0x00, 0x00, 0x00, 0x00};
unsigned char ClockPoint = 1;
bool times;

//unsigned char Update=0;


TM1637 tm1637(CLK, DIO);

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
#endif
  tm1637.set();
  tm1637.init();
  clock.begin();
  //  Timer1.initialize(1000000);//timing for 500ms
  // Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR
  pinMode(RELAYPIN, OUTPUT);
  times = true;
  digitalWrite(RELAYPIN, 1);
  wdt_enable(WDTO_1S);


}
void delays(unsigned long ms)
{
  uint16_t start = (uint16_t)micros();

  while (ms > 0)
  {
    if (((uint16_t)micros() - start) >= 1000)
    {
      ms--;
      start += 1000;
    }
    wdt_reset();
  }
}





void clickbell()
{
  int i;
  if (times)
  {

    times = false;
    for (i = 0; i < 3; i++)
    {
      digitalWrite(RELAYPIN, 0);
      delays(1560);
      digitalWrite(RELAYPIN, 1);
      delays(3000);
    }

  }
  //delays(2000);

}
void clickbell_morning()
{
  int i;
  if (times)
  {

    times = false;
    for (i = 0; i < 6; i++)
    {
      digitalWrite(RELAYPIN, 0);
      delays(1560);
      digitalWrite(RELAYPIN, 1);
      delays(3000);
    }

  }
  //delays(2000);

}
void check_time()
{
  clock.getTime();
  if ((clock.dayOfWeek != SAT) && (clock.dayOfWeek != FRI))
  {
    //////////Alarm 1 for children
    if ((clock.hour == 21) && (clock.minute == 20))
    {
      clickbell();
      delays(60000);
    }
    if ((clock.hour == 21) && (clock.minute > 30))
    {
      times = true;
    }
    //////////

  }
  else // SAT FRI
  {
    if ((clock.hour == 22) && (clock.minute == 00))
    {
      clickbell();
      delays(60000);
    }
    if ((clock.hour == 22) && (clock.minute > 00))
    {
      times = true;
    }
  }
  
  
    if ((clock.dayOfWeek != SUN) && (clock.dayOfWeek != SAT))
  {
    //////////Alarm 2 for morning
    if ((clock.hour == 7) && (clock.minute == 20))
    {
      clickbell_morning();
      delays(60000);
    }
    if ((clock.hour == 7) && (clock.minute > 20))
    {
      times = true;
    }
  }
  else //SUN SAT
  {   
    //////////Alarm 2 for morning
    if ((clock.hour == 8) && (clock.minute == 00))
    {
      clickbell_morning();
      delays(60000);
    }
    if ((clock.hour == 8) && (clock.minute > 00))
    {
      times = true;
    }
  
  
  }
  //////////
  //clickbell();


}

void loop()
{
  //clickbell();

#if 1
  ClockPoint = (~ClockPoint) & 0x01;
  time_display();
  check_time();


  delays(2000);
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
  delays(100);
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

