#include "avr.h"
#include <util/atomic.h>

#include "nametag.h"

volatile byte input_change = 0;
volatile byte input_old;
#define INPUT_MASK (0b111 << 1)

int main(void)
{
	init();

	// input buttons: pins PC 1-3
	DDRC &= ~INPUT_MASK;
	PORTC |= INPUT_MASK;
	input_old = PINC;

	// handle input buttons as pin-change interrupts
	PCMSK1 |= INPUT_MASK;
	PCICR |= 1 << PCIE1; // pin-change interrupt enable for PCI1

	NameTag m(1);
	m.setText("Hello");
	m.setShiftMode(NameTag::NO_SHIFT);
	while(1) {
		m.show();
		if (input_change)
			m.setColumn(7, (~input_old & INPUT_MASK) | (input_change << 4));
	}
}

ISR(PCINT1_vect)
{
	byte input_new = PINC;
	input_change = ((input_old ^ input_new) & INPUT_MASK) | 1;
	input_old = input_new;
}

