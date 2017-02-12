#include "nametag_sm.h"
#include "nametag.h"

#undef TRANSITION
#define TRANSITION(s) setState(STATE_CAST(&NameTagSM::s))

NameTagSM::NameTagSM(NameTag *display)
   : StateMachine(STATE_CAST(&NameTagSM::stateDefault))
   , display(display)
{
	// setup input pins
	DDRC &= ~INPUT_MASK;
	PORTC |= INPUT_MASK;

	name_text = "Tekkietorium - Felix Haschke & Fabian Umhang";
	display->setText(name_text);
	display->setShiftSpeed(15);
}

void NameTagSM::stateDefault(byte event)
{
	if (event & BTN_MENU) {
		time = millis();
		display->setChar('E', 0);
		TRANSITION(stateEnterMenu);
	} else
		display->update();
}

void NameTagSM::stateEnterMenu(byte event)
{
	// menu button still pressed ?
	if (event & BTN_MENU) {
		// pressed longer than 500ms?
		if (millis() > time + 500) {
			display->setText("Menu");
			TRANSITION(stateMenu);
		}
	} else { // otherwise: back to default state
		TRANSITION(stateDefault);
	}
}

void NameTagSM::stateMenu(byte event)
{
	if (event & (BTN_DOWN | BTN_UP)) {
		display->setText(name_text);
		TRANSITION(stateDefault);
	}
	display->update();
}
