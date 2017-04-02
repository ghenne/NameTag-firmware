#include "avr.h"
#include "nametag.h"

#define CHANGE   bit(3)
// these are the actual input pins (of PINC)
#define BTN_DOWN bit(0)
#define BTN_MENU bit(1)
#define BTN_UP   bit(2)
// mask of all input pins
#define INPUT_MASK (BTN_DOWN | BTN_UP | BTN_MENU)

byte clock_pin = 1;
byte latch_pin = 0;

int main(void)
{
	init();

	// input buttons: pins PC 1-3
	DDRB &= ~INPUT_MASK;
	PORTB |= INPUT_MASK;

	DDRC &= ~_BV(6);
	PORTC |= _BV(6); // pullup reset


	// handle input buttons as pin-change interrupts
	PCMSK1 |= INPUT_MASK;
	PCICR |= _BV(PCIE1); // pin-change interrupt enable for PCI1

	NameTag m(4);

	while(1) {
	}
}

void shift(byte value)
{
	byte portc = PORTC;
	bitWrite(portc, 2, value);
	bitWrite(portc, 3, value);
	bitWrite(portc, 4, value);
	bitWrite(portc, 5, value);
	PORTC = portc;
	bitClear(PORTC, clock_pin);
	bitSet(PORTC, clock_pin);
}

void latch()
{
	bitClear(PORTC, clock_pin);
	bitSet(PORTC, latch_pin);
	bitSet(PORTC, clock_pin);

	bitClear(PORTC, clock_pin);
	bitClear(PORTC, latch_pin);
	bitSet(PORTC, clock_pin);
}

ISR(PCINT1_vect) {
	delayMicroseconds(500);

	if (~PINB & BTN_DOWN)
		shift(0);
	else if (~PINB & BTN_UP)
		shift(1);
	else if (~PINB & BTN_MENU)
		latch();
}
