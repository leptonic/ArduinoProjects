/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

int RECV_PIN = 15;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  pinMode(5,OUTPUT);
  irrecv.enableIRIn(); // Start the receiver
  Keyboard.begin();
}
void enter_cmd()
{
  delay(50);  
  Keyboard.press(KEY_RETURN);
  delay(200);
  Keyboard.release(KEY_RETURN);
  delay(7000);
  Keyboard.press(0xc1);
  delay(100);
    Keyboard.release(0xc1);
    delay(200);
  Keyboard.press(KEY_LEFT_GUI);
  delay(150);
  Keyboard.press(114);
  delay(150);
  Keyboard.releaseAll();
  delay(150);
  Keyboard.print("CMD");
  delay(250);
  Keyboard.press(KEY_RETURN);
  delay(200);
  Keyboard.release(KEY_RETURN);
  delay(150);

}
void shutdown_s()
{
  delay(150);
  Keyboard.print("SHUTDOWN -S -T 0 ");
  delay(200);
  
  Keyboard.press(KEY_RETURN);
  delay(200);
  Keyboard.release(KEY_RETURN);
  delay(150);
}
void loop() {

digitalWrite(5,HIGH);

  if (irrecv.decode(&results)) {
  //  Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
  if(results.value==0xFFA25D)
  {
    enter_cmd();
    delay(200);
    shutdown_s();
    results.value=0;
   }
    if(results.value==0xFF629D)
    {
      digitalWrite(5,LOW);
      delay(1000);
      results.value=0;
     }


  
  delay(100);
}
