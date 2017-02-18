#include "avr.h"
#include "nametag.h"


#define INPUT_MASK (0b111 << 1)
volatile char num = 1;
volatile byte previous = 0;
volatile bool changed = true;

int main(void)
{
	init();

	// input buttons: pins PC 1-3
	DDRC &= ~INPUT_MASK;
	PORTC |= INPUT_MASK;

	// handle input buttons as pin-change interrupts
	PCMSK1 |= INPUT_MASK;
	PCICR |= 1 << PCIE1; // pin-change interrupt enable for PCI1

	NameTag m(1);
	char buf[10];
	while(1) {
		if (changed) {
			m.setText(m.formatInt(buf, 10, num));
			changed = false;
		}
		m.update();
		m.show();
	}
}

ISR(PCINT1_vect) {
	changed = true;
	delayMicroseconds(500);

	byte value = ~PINC;
	byte change = previous ^ value;
	previous = value;

	if (change & bit(1))
		++num;
	else if (change & bit(2))
		num = 0;
	else if (change & bit(3))
		--num;
	if (num > 9) num = 0;
	if (num < 0) num = 9;
}
