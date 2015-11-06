#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "DS1307.h"

DS1307 clock;//define a object of DS1307 class
#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
String comdata = "";
String temp;
int hs;
int ms;
bool st;
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(57600);
  clock.begin();
  hs=25;
  ms=61;
st=false;
}

void loop() {
  clock.getTime();
   set_off();
  //if((hs<24)&&(ms<60))
  {
  if ((clock.hour==hs)&&(clock.minute==ms))
   rainbow(300);
   //set_ok();
  }
  while (Serial.available() > 0)
  {
    comdata += char(Serial.read());
    delay(2);
  }
  if (comdata.length() > 0)
  {
    Serial.println(comdata);
    if ((comdata[1] == 'T')&&(comdata[2]=='M'))
    {
       clock.getTime();
       
      Serial.println(clock.hour);
      Serial.println(clock.minute);
      //set_ok();
    }
     if ((comdata[1] == 'S')&&(comdata[2]=='T'))
    {
      st=true;
      
    }
    if ((comdata[1] == 'I')&&(comdata[2]=='M'))
    {
      rainbowCycle(5);
      //set_ok();
    }
    if (comdata[1] == 'S') //AS2401E
    {
      temp.remove(0);
      temp += comdata[2];
      temp += comdata[3];
      hs = temp.toInt();
      temp.remove(0);
      temp += comdata[4];
      temp += comdata[5];
      ms = temp.toInt();
      temp.remove(0);
      Serial.print("ALARM:");
      Serial.println(hs);
      Serial.print(":");
      Serial.println(ms);
      if((hs>23)||(ms>59))
      {
          set_error();
      }
      else
      {
      if(comdata[6]=='E')
          set_ok();
      }
    }



    comdata = "";
  }

  //rainbowCycle(20);

}



void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      // strip.setPixelColor(i, Wheel2((i+j) & 255));
      strip.setPixelColor(i, Wheel2(j));
      delay(wait);
      if (st)
      {
        st=false;
      return;
      }
    }
    strip.show();

  }
   set_off();
}
void set_error()
{
  uint16_t i;

  for (i = 0; i < strip.numPixels(); i++) {
    // strip.setPixelColor(i, Wheel2((i+j) & 255));
    strip.setPixelColor(i, strip.Color(100, 0, 255));

  }
  strip.show();
  delay(1000);
  strip.clear();
  for (i = 0; i < strip.numPixels(); i++) {
    // strip.setPixelColor(i, Wheel2((i+j) & 255));
    strip.setPixelColor(i, strip.Color(0, 0, 0));

  }
  strip.show();
    for (i = 0; i < strip.numPixels(); i++) {
    // strip.setPixelColor(i, Wheel2((i+j) & 255));
    strip.setPixelColor(i, strip.Color(250, 0, 25));

  }
  strip.show();
  delay(1000);
  strip.clear();
  for (i = 0; i < strip.numPixels(); i++) {
    // strip.setPixelColor(i, Wheel2((i+j) & 255));
    strip.setPixelColor(i, strip.Color(0, 0, 0));

  }
  strip.show();
}
void set_ok()
{
  uint16_t i;

  for (i = 0; i < strip.numPixels(); i++) {
    // strip.setPixelColor(i, Wheel2((i+j) & 255));
    strip.setPixelColor(i, strip.Color(180, 200, 255));

  }
  strip.show();
  delay(1000);
  strip.clear();
  for (i = 0; i < strip.numPixels(); i++) {
    // strip.setPixelColor(i, Wheel2((i+j) & 255));
    strip.setPixelColor(i, strip.Color(0, 0, 0));

  }
  strip.show();
}
void set_off()
{
  int i;
  strip.clear();
  for (i = 0; i < strip.numPixels(); i++) {
    // strip.setPixelColor(i, Wheel2((i+j) & 255));
    strip.setPixelColor(i, strip.Color(0, 0, 0));

  }
  strip.show();
}
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
  strip.clear();
  for (i = 0; i < strip.numPixels(); i++) {
    // strip.setPixelColor(i, Wheel2((i+j) & 255));
    strip.setPixelColor(i, strip.Color(0, 0, 0));

  }
  strip.show();

}

uint32_t Wheel2(byte WheelPos) {
  int r, g, b; //rs,gs,bs;//rf,gf,bf;
  // rs=170;
  // gs=50;
  // bs=20;
  //rf=255;
  //gf=250;
  //bf=240;
  if (WheelPos < 20)
  {
    r = 115;
    g = 47;
    b = 34;
    strip.setBrightness(20);
  }
  else if ((WheelPos >= 20) && (WheelPos < 60))
  {
    r = 170;
    g = 50;
    b = 20;
    strip.setBrightness(60);
  }
  else if ((WheelPos >= 60) && (WheelPos < 80))
  {
    r = 184;
    g = 58;
    b = 18;
    strip.setBrightness(80);
  }
  else if ((WheelPos >= 80) && (WheelPos < 100))
  {
    r = 220;
    g = 80;
    b = 19;
    strip.setBrightness(100);
  }
  else if ((WheelPos >= 100) && (WheelPos < 120))
  {
    r = 235;
    g = 118;
    b = 25;
    strip.setBrightness(120);
  }
  else if ((WheelPos >= 120) && (WheelPos < 140))
  {
    r = 237;
    g = 128;
    b = 27;
    strip.setBrightness(140);
  }
  else if ((WheelPos >= 140) && (WheelPos < 160))
  {
    r = 230;
    g = 117;
    b = 11;
    strip.setBrightness(160);
  }
  else if ((WheelPos >= 160) && (WheelPos < 180))
  {
    r = 244;
    g = 185;
    b = 31;
    strip.setBrightness(180);
  }
  else if ((WheelPos >= 180) && (WheelPos < 200))
  {
    r = 250;
    g = 189;
    b = 46;
    strip.setBrightness(190);
  }
  else if ((WheelPos >= 200) && (WheelPos < 210))
  {
    r = 250;
    g = 195;
    b = 66;
    strip.setBrightness(210);
  }
  else if ((WheelPos >= 210) && (WheelPos < 220))
  {
    r = 250;
    g = 195;
    b = 66;
    strip.setBrightness(220);
  }

  else if ((WheelPos >= 220) && (WheelPos < 230))
  {
    r = 250;
    g = 200;
    b = 86;
    strip.setBrightness(230);
  }
  else if ((WheelPos >= 230) && (WheelPos < 240))
  {
    r = 250;
    g = 210;
    b = 110;
    strip.setBrightness(240);
  }
  else if ((WheelPos >= 240) && (WheelPos < 250))
  {
    r = 255;
    g = 230;
    b = 180;
    strip.setBrightness(255);
  }
  else if ((WheelPos >= 250) && (WheelPos < 252))
  {
    r = 255;
    g = 235;
    b = 200;
    strip.setBrightness(255);
  }
  else if ((WheelPos >= 252) && (WheelPos < 254))
  {
    r = 255;
    g = 240;
    b = 220;
    strip.setBrightness(255);
  }
  else if (WheelPos >= 254)
  {
    r = 255;
    g = 250;
    b = 240;
    strip.setBrightness(255);
  }
  return strip.Color(r, g, b);
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
void printTime()
{
  clock.getTime();
  Serial.print(clock.hour, DEC);
  Serial.print(":");
  Serial.print(clock.minute, DEC);
  Serial.print(":");
  Serial.print(clock.second, DEC);
  Serial.print("	");
  Serial.print(clock.month, DEC);
  Serial.print("/");
  Serial.print(clock.dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(clock.year + 2000, DEC);
  Serial.print(" ");
  switch (clock.dayOfWeek)// Friendly printout the weekday
  {
    case MON:
      Serial.print("MON");
      break;
    case TUE:
      Serial.print("TUE");
      break;
    case WED:
      Serial.print("WED");
      break;
    case THU:
      Serial.print("THU");
      break;
    case FRI:
      Serial.print("FRI");
      break;
    case SAT:
      Serial.print("SAT");
      break;
    case SUN:
      Serial.print("SUN");
      break;
  }
  Serial.println(" ");
}
