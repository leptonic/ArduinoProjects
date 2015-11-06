#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <HMC5883L.h>
HMC5883L compass;
#define PIN 6

int direct;
  float xDegrees ; 
  float yDegrees ; 
  float zDegrees;
  
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
   Serial.begin(9600);
  Wire.begin();
  compass = HMC5883L();
  compass.SetScale(1.3);
  compass.SetMeasurementMode(Measurement_Continuous);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}
void HX5883L()
{
 MagnetometerRaw raw = compass.ReadRawAxis();
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  float xHeading = atan2(scaled.YAxis, scaled.XAxis);
  float yHeading = atan2(scaled.ZAxis, scaled.XAxis);
  float zHeading = atan2(scaled.ZAxis, scaled.YAxis);
  if(xHeading < 0)    xHeading += 2*PI;
  if(xHeading > 2*PI)    xHeading -= 2*PI;
  if(yHeading < 0)    yHeading += 2*PI;
  if(yHeading > 2*PI)    yHeading -= 2*PI;
  if(zHeading < 0)    zHeading += 2*PI;
  if(zHeading > 2*PI)    zHeading -= 2*PI;
   xDegrees = xHeading * 180/M_PI; 
   direct=xDegrees/30+1;
   if(direct==12)
   direct=0;
   
   yDegrees = yHeading * 180/M_PI; 
   zDegrees = zHeading * 180/M_PI; 
  delay(100);
}
void loop() {
  HX5883L();
  // Some example procedures showing how to display to the pixels:
 // colorWipe(strip.Color(255, 0, 0), 50); // Red
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  //colorWipe(strip.Color(0, 0, 255), 50); // Blue
  // Send a theater pixel chase in...
  //theaterChase(strip.Color(127, 127, 127), 50); // White
  //theaterChase(strip.Color(127,   0,   0), 50); // Red
  //theaterChase(strip.Color(  0,   0, 127), 50); // Blue
//colorWipe(strip.Color(255, 0, 0), 50); // Red
  for(uint16_t i=0; i<12; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));       
  }
    strip.setPixelColor(direct, Wheel360((int)yDegrees));
    strip.setBrightness(Brightness(zDegrees));
      strip.show();

  //rainbow(20);
  //rainbowCycle(20);
  //theaterChaseRainbow(50);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<10; i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
uint32_t Wheel360(int WheelPos) {

  if(WheelPos <= 120)
  {
   return strip.Color(255 - WheelPos * 2, 0, WheelPos * 2);
  } 
  
  else if((WheelPos >120)&&(WheelPos <=240))
  {
   
   return strip.Color(0, WheelPos * 2, 255 - WheelPos * 2);
  }   
  else if((WheelPos >240)&&(WheelPos < 361))
  {
    
   return strip.Color(255 - WheelPos * 2, WheelPos * 2, 0);
  }
}
uint8_t Brightness(float z)
{
int val;
val=(int)z;
if(val>255)
return 0xff;
else
return val;



}
