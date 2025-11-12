#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3
#define CLK2 4
#define DIO2 5

TM1637Display display1(CLK, DIO);
TM1637Display display2(CLK2, DIO2);



const int potPin = A0;
const int potPin2 = A1; 
int value;
int value2;

void setup() {
  pinMode(potPin, INPUT);
  pinMode(potPin2, INPUT);

  display1.setBrightness(0x0f);
  display2.setBrightness(0x0f);

}

void loop() {
  value = analogRead(potPin);
  value2 = analogRead(potPin2);
  display1.showNumberDec(value, false); // shows 0–1023
  delay(100);
  display2.showNumberDec(value2, false); // shows 0–1023
  delay(100);
}
