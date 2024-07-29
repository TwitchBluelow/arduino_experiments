/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
int led = 13; 

void setup() { 
// initialize the digital pin as an output.  
  pinMode(led, OUTPUT);
}

void loop() { 
  digitalWrite(led, HIGH); 
  delay(1000);  
  digitalWrite(led, LOW); 
  delay(1000);  
} 
