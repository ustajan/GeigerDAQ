// Authors:  Areg Hovhannisyan, Areg Danagoulian
// email: areg_hovhannisyan@edu.aua.am, aregjan@mit.edu
// Controbutions from:  Gagik Nersisyan
// License: Attribution 4.0 International (CC BY 4.0)
// Description: detects TTL pulses from the Geiger-Muller detector, 
// captures the time of the event in microseconds, and sends it to the computer over the serial connection.
// Performs preliminary analysis and displays those on an oled display. 

// include the library code:

#include <Wire.h>
#include <Adafruit_GFX.h> //OLED
#include <Adafruit_SSD1306.h> // for more info on how to use it refer to https://randomnerdtutorials.com/guide-for-oled-display-with-arduino/

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to

//LiquidCrystal_I2C lcd(0x27,16,2); // to ttalk to the LCD over i2c
// for pinout see https://www.makerguides.com/character-i2c-lcd-arduino-tutorial/

const byte interruptPin = 0; //0 is also marked as RX1 on Arduino Micro Pro, RX0 on Nano Every
volatile uint32_t stack[128]; //it looks like anything larger 128 hangs the OLED
volatile uint8_t stack_top = 0;
uint32_t count=0;
int display_refresh_time=5; //in seconds
unsigned long last_time=0;
float max_dose=0; //maximum observed dose, in uR/hr
float total_dose=0; //total dose, in uR

#define LED 9 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

void loop() {
	while (stack_top) {
		Serial.write((uint8_t*) &stack[--stack_top], 4);
	}

  unsigned long time=(millis()/1000L);
  
  if(time-last_time>display_refresh_time){
    display_counts(time-last_time);
    last_time=time;
  }
  if(millis()%2000==0) display_voltage(); //once per 2s display voltages
}

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
//  pinMode(buttonPin,INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(buttonPin), SetMode, FALLING); // trigger when button pressed, but not when released
  
	pinMode(interruptPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(interruptPin), detect, RISING);
	Serial.begin(115200);


  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) { // Address 0x3c for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  // Display static text
  display.println("Welcome to MIT LANPh Geiger world!");
  display.println();
  display.println("For more info go to");
  display.println();
  display.println("areg.mit.edu");
  display.display(); 
  delay(4000);
  display.clearDisplay();
    
 // LCD display:
 // lcd.init(); // this is for i2c only
 // lcd.backlight(); // this is for i2c only

  analogReference(INTERNAL); //this sets the ADC reference voltage to 2.56 V for Pro Micro. For more see https://www.arduino.cc/reference/en/language/functions/analog-io/analogreference/
  display_voltage();
  
  // set up the LED
  pinMode(LED, OUTPUT);
}

void detect() {
  if(stack_top<128){
  	stack[stack_top] = micros();
  	++stack_top;
  }
  ++count;
  digitalWrite(LED, HIGH);
  delay(1);                //this is gonna be a deadtime.  Turn on only when rates are low.
  digitalWrite(LED, LOW);

}

void display_counts(unsigned long time_difference) {


    display.setTextSize(2);
    display.setTextColor(WHITE,BLACK);
    display.setCursor(0,13);
    display.print("          ");     display.setCursor(0,13); //erase the line
    display.print("CPM:");
    float CPM=float(count)*60/(time_difference);
    display.print(uint32_t(CPM));
    display.display();

    display.setCursor(0,28);
    display.setTextColor(WHITE,BLACK);
    display.print("          ");     display.setCursor(0,28); //erase the line
    display.print("uR/hr:");
    float dose=CPM*0.57;  //0.0057 is the conversion from CPM to uSv/hr for the SBM-20 unit (0.0066 for the J305)
    display.print(uint32_t(dose)); 
    display.display();
    if(dose>max_dose) max_dose=dose;
    total_dose+=dose*time_difference/3600L; //dose rate times the claculation window

    //now display the max dose on the first row:
    display.setTextSize(1);
    display.setTextColor(WHITE,BLACK);
    display.setCursor(0,44);
    display.print("                   ");     display.setCursor(0,44); //erase the line
    display.print("max:");
    display.print(max_dose);
    display.setCursor(0,55);
    display.print("                   ");     display.setCursor(0,55); //erase the line    
    display.print("TOTAL:");
    display.print(total_dose);
    
    count=0; //reset
    if(CPM<20) display_refresh_time=30; //depending on the current rate, change the integration time
    else if(CPM<100) display_refresh_time=10;
    else display_refresh_time=1;

 
}

void display_voltage(){
//  for(int i=0;i<5;++i) analogRead(1);
  display.setTextSize(1);
  display.setTextColor(WHITE,BLACK);
  display.setCursor(0,0);
  display.print("                   ");     display.setCursor(0,0); //erase the line
  display.print("VCC:");
  display.print(2L*analogRead(1)*2.56/1024); //print the VCC voltage. Assumes that the voltage has been divided by a voltage divider, by 2.  Max value of input is 2.56V.
  display.print("V");
//  display.setCursor(0,12);
//  display.print("                   ");     display.setCursor(0,12); //erase the line
  display.print("|HV:");
  for(int i=0;i<5;++i) analogRead(2);
  display.print(909L*analogRead(2)*2.56/1024); //print the HV.  Assumes a voltage divider of 0.0011
  display.print("V");
  
  display.display();
  
}
