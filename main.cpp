#include "avr.h"
#include "nametag.h"
#include "nametag_sm.h"

int main(void)
{
	init();

	NameTag m(1);
	NameTagSM sm(&m);
	while(1) {
		sm.process(~PINC & INPUT_MASK);
		m.show();
	}
}
