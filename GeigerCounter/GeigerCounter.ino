// Author: Authors:  Areg Hovhannisyan
// email: areg_hovhannisyan@edu.aua.am
// License: Attribution 4.0 International (CC BY 4.0)
// Description: detects TTL pulses from the Geiger-Muller detector, 
// captures the time of the event in microseconds, and sends it to the computer over the serial connection

// include the library code:
//#include <LiquidCrystal.h>   //for bare LCD
#include <LiquidCrystal_I2C.h> //this is for the LCD with an i2c backpack

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 6, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //bare LCD
LiquidCrystal_I2C lcd(0x27,16,2); // to ttalk to the LCD over i2c

const byte interruptPin = 0; //0 is also marked as RX1 on Arduino Micro Pro, RX0 on Nano Every
volatile uint32_t stack[256];
volatile uint8_t stack_top = 0;
uint32_t count=0;
uint32_t count_long=0;
int display_refresh_time=5; //in seconds
int display_refresh_long=30;

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
  lcd.init(); // this is for i2c only
  lcd.backlight(); // this is for i2c only
  
  // set up the LCD's number of columns and rows:
  // lcd.begin(16, 2); //this is for direct LCD only
  // Print a message to the LCD.
  lcd.print("CPM: summing");
  lcd.setCursor(0,1);
  lcd.print("uR/hr: summing");
  lcd.display();
 
}

void detect() {
	stack[stack_top] = micros();
	++stack_top;
  ++count;
  ++count_long;
}

void display() {

  float CPM, CPM_long;
  unsigned long time=millis();
  
  if(time%(display_refresh_time*1000)==0){
    lcd.setCursor(4,0); // go one past (cpm):
    lcd.print("          ");
    lcd.setCursor(4,0); // go one past (cpm):
    CPM=float(count)*60/(display_refresh_time);
    lcd.print(int(CPM));
    count=0; //reset
    if(CPM<20) display_refresh_time=30; //depending on the current rate, change the integration time
    else if(CPM<100) display_refresh_time=10;
    else display_refresh_time=1;
  }
  if(time%(display_refresh_long*1000)==0){ //for the dose we use the longer, 30sec integration
    lcd.setCursor(7,1); // go one past /hr:
    lcd.print("         ");
    lcd.setCursor(7,1); // go one past /hr:
    CPM_long=float(count_long)*60/(display_refresh_long);
    lcd.print(int(CPM_long*0.57)); //0.0057 is the conversion from CPM to uSv/hr for the SBM-20 unit (0.0066 for the J305)
    count_long=0;
  }  
}
