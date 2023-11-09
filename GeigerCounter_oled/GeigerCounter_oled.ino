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
uint32_t count=0,count_total=0;
uint32_t count_refresh = 5;
int display_refresh_time=1; //in seconds
unsigned long last_time=0,last_led_on=0;
float max_dose=0; //maximum observed dose, in uR/hr
float total_dose=0; //total dose, in uR
bool first_time=true;
bool led_blink_on=false,led_blink_arm=true;

#define LED 9 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void loop() {
	while (stack_top) {
		Serial.write((uint8_t*) &stack[--stack_top], 4);
	}
  unsigned long time=(millis()/1000L);
  
  if(time-last_time>display_refresh_time && count>=count_refresh){
    display_counts(time-last_time);
    last_time=time;
  }
  if(millis()%2000==0 && first_time==false) display_voltage(); //once per 2s display voltages

  blinker();
}


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
  //Welcome message
  display.println("Laboratory of Applied Nuclear Physics");
  display.println();
  display.println("Visit us at");
  display.println();
  display.println("lanph.mit.edu");
  display.println();
  display.println("Counting...pls wait.");
  display.display(); 
  analogReference(INTERNAL); //this sets the ADC reference voltage to 2.56 V for Pro Micro. For more see https://www.arduino.cc/reference/en/language/functions/analog-io/analogreference/
  
  // set up the LED
  pinMode(LED, OUTPUT);
}

void detect() {
  if(stack_top<256){
  	stack[stack_top] = micros();
  	++stack_top;
  }
  ++count; //this gets reset every time the rates are displayed, in display_count()
  ++count_total;
  led_blink_on=true;
  
}

void display_counts(unsigned long time_difference) {

  if(first_time) {
    display.clearDisplay();
    first_time=false;
  }
    

  display.setCursor(0,9);
  display.setTextSize(1);    
  display.print("                   ");     display.setCursor(0,9); //erase the line    
  display.print("count/time:");
  display.print(count_total);
  display.print("/");
  display.print(millis()/1000L);



  display.setTextSize(2);
  display.setCursor(0,18);
  display.print("          ");     display.setCursor(0,18); //erase the line
  display.print("CPM:");
  float CPM=float(count)*60/(time_difference);
  display.print(uint32_t(CPM));

  display.setCursor(0,33);
  display.print("          ");     display.setCursor(0,33); //erase the line
  display.print("uR/hr:");
  float dose_rate=CPM*0.57;  //0.0057 is the conversion from CPM to uSv/hr for the SBM-20 unit (0.0066 for the J305)
  display.print(uint32_t(dose_rate)); 
  if(dose_rate>max_dose) max_dose=dose_rate;
  total_dose+=dose_rate*time_difference/3600L; //dose rate times the claculation window

  //now display the max dose:
  display.setTextSize(1);
  display.setCursor(0,48);
  display.print("                   ");     display.setCursor(0,48); //erase the line
  display.print("max:");
  display.print(max_dose);
  
  display.setCursor(0,56);
  display.print("                   ");     display.setCursor(0,56); //erase the line
  display.print("Dose(uR):");
  display.print(total_dose);

  display.display();

  
  count=0; //reset
 
}

void display_voltage(){

  display.setTextSize(1);
  display.setTextColor(WHITE,BLACK);
  display.setCursor(0,0);
  display.print("                   ");     display.setCursor(0,0); //erase the line
  display.print("VCC:");
  display.print(2L*analogRead(1)*2.56/1024); //print the VCC voltage. Assumes that the voltage has been divided by a voltage divider, by 2.  Max value of input is 2.56V.
  display.print("V");
  display.print("    HV:");
  for(int i=0;i<5;++i) analogRead(2);
  display.print(int(909L*analogRead(2)*2.56/1024)); //print the HV.  Assumes a voltage divider of 0.0011
  display.print("V");
  
  display.display();
  
}
void blinker(){ //operate the LED on/off

  if(led_blink_on && led_blink_arm){
   digitalWrite(LED, HIGH);
   led_blink_arm=false;
   last_led_on=millis();
  }
  else if(led_blink_on && millis()-last_led_on>10){ //stay on for 10 ms
    digitalWrite(LED, LOW);
    led_blink_arm=true;
    led_blink_on=false;     
  }
  
}
