#include "avr.h"
#include "nametag.h"
#include "nametag_sm.h"

FUSES = {
   LFUSE_DEFAULT | (byte)~FUSE_CKDIV8, // run at 8MHz
   HFUSE_DEFAULT & FUSE_EESAVE, // protect EEPROM from erase
   EFUSE_DEFAULT,
};

volatile byte have_input = 0;

int main(void)
{
	init();

	// input buttons: pins PB 0-2
	DDRB &= ~INPUT_MASK;
	PORTB |= INPUT_MASK;
	PORTC |= _BV(6); // pullup reset


	// handle input buttons as pin-change interrupts
	PCMSK0 |= INPUT_MASK;
	PCICR |= _BV(PCIE0); // pin-change interrupt enable for PCI1

	NameTag m(4);
	NameTagSM sm(&m);

	while(1) {
		sm.process(have_input | (~PINB & INPUT_MASK));
		have_input = 0;
		m.show();
	}
}

ISR(PCINT0_vect) {
	delayMicroseconds(500);
	have_input = CHANGE;
}
