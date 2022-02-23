// Author: Authors:  Areg Hovhannisyan
// email: areg_hovhannisyan@edu.aua.am
// License: Attribution 4.0 International (CC BY 4.0)
// Description: detects TTL pulses from the Geiger-Muller detector, 
// captures the time of the event in microseconds, and sends it to the computer over the serial connection

const byte interruptPin = 2;

volatile uint32_t stack[256];
volatile uint8_t stack_top = 0;

void loop() {
	while (stack_top) {
		Serial.write((uint8_t*) &stack[--stack_top], 4);
	}
}

void setup() {
	pinMode(interruptPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(interruptPin), detect, RISING);
	Serial.begin(115200);
}

void detect() {
	stack[stack_top] = micros();
	++stack_top;
}
