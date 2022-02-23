//#include <TimeLib.h>

const byte interruptPin = 2;

void loop() {
}

void setup() {
	pinMode(interruptPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(interruptPin), notify, RISING);
	Serial.begin(115200);
//        unsigned long t = now();

        
}

void notify() {
//	Serial.write('a');
        unsigned long time = micros();
        Serial.println(time);
//        Serial.print(" ");
}
