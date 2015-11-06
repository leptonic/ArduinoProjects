int BUZZERPin = 13;   // LED 连接到数字端口13
int sensor = 2;    //传感器连接到数字端口2
//红外热释电运动传感器发出高电平，则被触发
//发出低电平，则其处于休眠状态
int val = 0; //存储传感器输出的数字量
void setup() {
  // put your setup code here, to run once:

 pinMode(BUZZERPin, OUTPUT);      // sets the digital pin as output
  pinMode(sensor, INPUT);       // sets the digital pin as input
  digitalWrite(BUZZERPin,HIGH);    //初始化熄灭灯
  Serial.begin(9600);//设置串行通信的波特率

}
void open_phone()
{

  digitalWrite(BUZZERPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);              // wait for a second
  digitalWrite(BUZZERPin, LOW);    // turn the LED off by making the voltage LOW
  delay(200); 
  digitalWrite(BUZZERPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);              // wait for a second
  digitalWrite(BUZZERPin, LOW);    // turn the LED off by making the voltage LOW
  delay(200);  
pinMode(5, OUTPUT);
 digitalWrite(5, LOW); 
 delay(500);
 pinMode(5, INPUT);
 delay(5000);
}
void loop() {
  // put your main code here, to run repeatedly:
  pinMode(5, INPUT);
val = digitalRead(sensor);   // read the input pin
  //如果红外热释电运动传感器被触发，将保持高电平一段时间
  if(HIGH == val)
  {
   // Serial.write(1);//向Processing发出传感器被触发标志
    digitalWrite(BUZZERPin,LOW);   // LED亮
    open_phone();
    delay(100);//延时，以使串行通讯不至于太忙
  }
  else 
  {digitalWrite(BUZZERPin,HIGH);    // LED灭
  //Serial.write((byte)0);//向Processing发出传感器休眠标志
  delay(100);
  }
}
