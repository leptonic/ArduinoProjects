#include <Adafruit_NeoPixel.h>
int max;
int min;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 3, NEO_GRB + NEO_KHZ800);
int speed;
int  old_speed;
void setup() {
  max = 0;
  min = 500;
  speed = 0;
  old_speed = 0;
  Serial.begin(9600);         //使用9600速率进行串口通讯
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void speed_test()
{

  int data;    //读取A0口的电压值
  int timeout;

  timeout = 0;
  data = 0;

  while (timeout < 12)
  {
    data = analogRead(A7);

    if (data > max)
      max = data;
    if (data < min)
      min = data;
    if ((max - min) > 2) // 1/4 cycle 18
    {
      max = 0;
      min = 500;
      speed++;
    }

    timeout++;
    delay(20);
  }



}
void openled(int sp)
{

  if (sp < 3)
  {
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.setBrightness(0);
  }
  else
  {
     
     
    if(sp<10)
    {
        strip.setBrightness(sp * 10);
      strip.setPixelColor(0, strip.Color(0, 0, sp * 25));
    }
    else if((sp>9)&&(sp<13))
    {
      sp=sp-9;
            strip.setBrightness(150+sp*15);
      strip.setPixelColor(0, strip.Color(0, sp*80, 255));
    }
        else if((sp>12)&&(sp<19))
    {
      sp=sp-19;
            strip.setBrightness(200+sp*9);
      strip.setPixelColor(0, strip.Color(sp*60, 255, 255));
    }
    else
    {
          strip.setPixelColor(0, strip.Color(255, 255, 255));
      strip.setBrightness(254);
    }
    
 #if  0
 s
    if (sp < 26)
    {
      if (sp <= 13)
      {
        strip.setBrightness(sp * 10);
        sp = sp - 3;
        strip.setPixelColor(0, strip.Color(0, 0, sp * 25));

      }
      else if ((sp > 13) && (sp <= 23))
      {
        strip.setBrightness(sp * 10);
        sp = sp - 13;
        strip.setPixelColor(0, strip.Color(0, sp * 25, 255));

      }
      else //>23_25
      {
        strip.setBrightness(sp * 10);
        strip.setPixelColor(0, strip.Color(sp * 10, 255, 255));

      }

    }
    else
    {
      strip.setPixelColor(0, strip.Color(255, 255, 255));
      strip.setBrightness(254);
    }
 #endif
  }
  strip.show();
}
void brakeled()
{
  strip.setPixelColor(0, strip.Color(255, 0, 0));
  strip.setBrightness(255);
  strip.show();
  delay(1500);
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.setBrightness(0);
  strip.show();




}

void loop() {


  speed_test();
  if ((old_speed - speed) > 2)  
  {
    brakeled();
  }
  else
  {

    // Serial.println("s");                   //串口输出温度数据
    openled(speed);
  }
  old_speed = speed;
  speed = 0;
  //delay(100);                           //等待2秒，控制刷新速度
}
