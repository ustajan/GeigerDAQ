// Author: Authors:  Areg Hovhannisyan
// email: areg_hovhannisyan@edu.aua.am
// License: Attribution 4.0 International (CC BY 4.0)
// Description: detects TTL pulses from the Geiger-Muller detector, 
// captures the time of the event in microseconds, and sends it to the computer over the serial connection

// include the library code:
#include <LiquidCrystal.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 6, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte interruptPin = 12; //0 is also marked as RX1 on Arduino Micro.  On nano every we use 12 (no zero??)

volatile uint32_t stack[256];
volatile uint8_t stack_top = 0;
uint32_t count=0;
int display_refresh_time=5; //in seconds

void loop() {
	while (stack_top) {
		Serial.write((uint8_t*) &stack[--stack_top], 4);
	}

  display();
}

void setup() {
	pinMode(interruptPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(interruptPin), detect, RISING);
	Serial.begin(115200);

 // LCD display:
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("CPM:");
  lcd.setCursor(0,1);
  lcd.print("uR/hr:");
  lcd.display();
 
}

void detect() {
	stack[stack_top] = micros();
	++stack_top;
  ++count;
}

void display() {

  
  if(millis()%(display_refresh_time*1000)==0){
    lcd.setCursor(4,0); // go one past (cpm):
    float CPM=float(count)*60/(display_refresh_time);
    lcd.print("          ");
    lcd.setCursor(4,0); // go one past (cpm):
    lcd.print(int(CPM));
    lcd.setCursor(7,1); // go one past /hr:
    lcd.print("          ");
    lcd.setCursor(7,1); // go one past /hr:
    lcd.print(CPM*0.57); //0.0057 is the conversion from CPM to uSv/hr
    count=0; //reset
    if(CPM<20) display_refresh_time=10;
    else if(CPM<100) display_refresh_time=5;
    else display_refresh_time=2;
  }
}
