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
// const int rs = 6, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //bare LCD
LiquidCrystal_I2C lcd(0x27,16,2); // to ttalk to the LCD over i2c
// for pinout see https://www.makerguides.com/character-i2c-lcd-arduino-tutorial/

const byte interruptPin = 0; //0 is also marked as RX1 on Arduino Micro Pro, RX0 on Nano Every
volatile uint32_t stack[256];
volatile uint8_t stack_top = 0;
uint32_t count=0;
uint32_t count_long=0;
int display_refresh_time=5; //in seconds
int display_refresh_long=30;

//const byte buttonPin    = 10; // I chose 10 because on pro Micro it's at the corner
//bool button=false; //button status

#define LED 9 

void loop() {
	while (stack_top) {
		Serial.write((uint8_t*) &stack[--stack_top], 4);
	}

  display_counts();
}

void setup() {
//  pinMode(buttonPin,INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(buttonPin), SetMode, FALLING); // trigger when button pressed, but not when released
  
	pinMode(interruptPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(interruptPin), detect, RISING);
	Serial.begin(115200);

 // LCD display:
  lcd.init(); // this is for i2c only
  lcd.backlight(); // this is for i2c only

  analogReference(INTERNAL); //this sets the ADC reference voltage to 2.56 V for Pro Micro. For more see https://www.arduino.cc/reference/en/language/functions/analog-io/analogreference/
  for(int i=0;i<5;++i) analogRead(1);
  lcd.setCursor(0,1);
  lcd.print("VCC: ");
  lcd.print(2L*analogRead(1)*2.56/1024); //print the VCC voltage. Assumes that the voltage has been divided by a voltage divider, by 2
  lcd.setCursor(0,0);
  lcd.print("HV: ");
  for(int i=0;i<50;++i) analogRead(2);
  lcd.print(909L*analogRead(2)*2.56/1024); //print the HV.  Assumes a voltage divider of 0.0011
  lcd.display();
  
  // set up the LED
  pinMode(LED, OUTPUT);
}

void detect() {
	stack[stack_top] = micros();
	++stack_top;
  ++count;
  ++count_long;
  digitalWrite(LED, HIGH);
  delay(1);                //this is gonna be a deadtime.  Turn on only when rates are low.
  digitalWrite(LED, LOW);

}

void display_counts() {

  float CPM, CPM_long;
  unsigned long time=millis();
  
  if(time%(display_refresh_time*1000)==0){
    lcd.setCursor(0,0);
    lcd.print("CPM: ");
    lcd.setCursor(4,0); // go one past (cpm):
    lcd.print("          ");
    lcd.setCursor(4,0); // go one past (cpm):
    CPM=float(count)*60/(display_refresh_time);
    lcd.print(int(CPM));
//    lcd.print(int(button));
    count=0; //reset
    if(CPM<20) display_refresh_time=30; //depending on the current rate, change the integration time
    else if(CPM<100) display_refresh_time=10;
    else display_refresh_time=1;
  }
  if(time%(display_refresh_long*1000)==0){ //for the dose we use the longer, 30sec integration
    lcd.setCursor(0,1);
    lcd.print("uR/hr: ");
    lcd.setCursor(7,1); // go one past /hr:
    lcd.print("         ");
    lcd.setCursor(7,1); // go one past /hr:
    CPM_long=float(count_long)*60/(display_refresh_long);
    lcd.print(int(CPM_long*0.57)); //0.0057 is the conversion from CPM to uSv/hr for the SBM-20 unit (0.0066 for the J305)
    count_long=0;
  }  
}

//int getBandgap(void) // Returns actual value of Vcc (x 100) //commenting this out for now, perhaps to be used in the future
//    {
//        
//#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
//     // For mega boards
//     const long InternalReferenceVoltage = 1115L;  // Adjust this value to your boards specific internal BG voltage x1000
//        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc reference
//        // MUX4 MUX3 MUX2 MUX1 MUX0  --> 11110 1.1V (VBG)         -Selects channel 30, bandgap voltage, to measure
//     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR)| (0<<MUX5) | (1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
//  
//#else
//     // For 168/328 boards
//     const long InternalReferenceVoltage = 1056L;  // Adjust this value to your boards specific internal BG voltage x1000
//        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc external reference
//        // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)         -Selects channel 14, bandgap voltage, to measure
//     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
//       
//#endif
//     delay(50);  // Let mux settle a little to get a more stable A/D conversion
//        // Start a conversion  
//     ADCSRA |= _BV( ADSC );
//        // Wait for it to complete
//     while( ( (ADCSRA & (1<<ADSC)) != 0 ) );
//        // Scale the value
//     int results = (((InternalReferenceVoltage * 1024L) / ADC) + 5L) / 10L; // calculates for straight line value 
//     return results*10; //to milivolts
// 
//    }
//
//void SetMode(void){
//  if(button)
//    button=false;
//  else
//    button=true;
//
//
//}
