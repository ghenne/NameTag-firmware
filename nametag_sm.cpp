#include "nametag_sm.h"
#include "nametag.h"

#include "avr/eeprom.h"

#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&NameTagSM::s)); \
	process(ENTER); \
}

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

void NameTagSM::stateDefault(byte event) // default state: print name
{
	if (event == ENTER) {
		display->setText(name_text);
		return;
	}

	if (event & BTN_MENU)
		TRANSITION(stateEnterMenu)
	else
		display->update();
}

void NameTagSM::stateEnterMenu(byte event) // option state enter menu
{
	if (event == ENTER) {
		time = millis();
		return;
	}

	// menu button still pressed ?
	if (event & BTN_MENU) {
		// pressed longer than 500ms?
		if (millis() > time + 500)
			TRANSITION(stateMainMenu);
	} else { // otherwise: back to default state
		// do not call ENTER
		setState(STATE_CAST(&NameTagSM::stateDefault));
		// TRANSITION(stateDefault);
	}
}

void NameTagSM::stateMainMenu(byte event)
{
	static const char* mainMenuText[] = {"Language", "Display", "Settings"};
	static char mainMenuItem = 1;

	if (event == ENTER)
		menuEntries = mainMenuText;
	else if (event & CHANGE) { // only react to input changes
		switch (event & INPUT_MASK) {
		case BTN_DOWN:
			if (--mainMenuItem < 0)
				mainMenuItem = 2;
			break;
		case BTN_UP:
			if (++mainMenuItem >= 3)
				mainMenuItem = 0;
			break;
		case BTN_MENU:
			switch (mainMenuItem) {
			case 0:
			case 1:
			case 2:
				TRANSITION(stateDefault);
				break;
			}
		default:
			return; // all buttons released
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setText(mainMenuText[mainMenuItem]);
}

#if 0
void eepromSetName (const char* text, byte number) //writes a name into the eeprom
{
	void eeprom_write_block(text, 512 + 64*number, min(64, strlen(text)));
}

void eepromReadName(number) {
	const char* text;
	void eeprom_read_block(text, 512 + 64*number, min(64, strlen(text)));
}
#endif
