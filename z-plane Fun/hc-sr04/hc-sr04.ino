
const int TrigPin = 2; 
const int EchoPin = 3; 
const int MotorControlPin = 6; 
float cm; 
void setup() 
{ 
Serial.begin(9600); 
pinMode(TrigPin, OUTPUT); 
pinMode(EchoPin, INPUT); 
pinMode(MotorControlPin, OUTPUT);

} 
void loop() 
{ 
  //digitalWrite(MotorControlPin, HIGH);
   ultrasonic();

}

//Fouctions
void ultrasonic()
{
digitalWrite(TrigPin, LOW); //低高低电平发一个短时间脉冲去TrigPin 
delayMicroseconds(2); 
digitalWrite(TrigPin, HIGH); 
delayMicroseconds(10); 
digitalWrite(TrigPin, LOW); 
 
cm = pulseIn(EchoPin, HIGH) / 58.0; //将回波时间换算成cm 
cm = (int(cm * 100.0)) / 100.0; //保留两位小数 
if (cm<40)
digitalWrite(MotorControlPin, LOW);
else
 digitalWrite(MotorControlPin, HIGH);

 Serial.print("ss");
Serial.print(cm);
Serial.println(";");

  delay(1000);

}

