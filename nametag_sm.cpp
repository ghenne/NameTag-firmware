#include "nametag_sm.h"
#include "nametag.h"

#include "avr/eeprom.h"

#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&NameTagSM::s)); \
	process(ON_ENTRY); \
}

NameTagSM::NameTagSM(NameTag *display)
   : StateMachine(STATE_CAST(&NameTagSM::stateDefault))
   , display(display), language(ENGLISH)
{
	// setup input pins
	DDRC &= ~INPUT_MASK;
	PORTC |= INPUT_MASK;

	name_text = "Tekkietorium - Felix Haschke & Fabian Umhang";
	display->setText(name_text);
	display->setShiftSpeed(5);
}

void NameTagSM::stateDefault(byte event) // default state: print name
{
	if (event == ON_ENTRY) {
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
	if (event == ON_ENTRY) {
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
	static const char* menuText[2][4] = {{"Language", "Display", "Settings","Return"},
	                                     {"Sprache", "Anzeige", "Einstellungen","Zurück"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		menuItem = 2;

	else if (event & CHANGE) { // only react to input changes
		switch (event & INPUT_MASK) {
		case BTN_DOWN:
			if (--menuItem < 0)
				menuItem = 3;
			break;
		case BTN_UP:
			if (++menuItem > 3)
				menuItem = 0;
			break;
		case BTN_MENU:
			switch (menuItem) {
			case 0:
				TRANSITION(stateLanguageMenu);
				break;
			case 1:
				// TRANSITION(stateDisplayMenu);
				break;
			case 2:
				TRANSITION(stateSettingsMenu);
				break;
			case 3:
				TRANSITION(stateDefault);
				break;
			}
		default:
			return;
		}
	} else {
		display->update();
		return;
	}
	display->setText(menuText[language][menuItem]);
}

void NameTagSM::stateSettingsMenu(byte event){

	static const char* menuText[2][3] = {{"Shiftmode", "Shiftspeed", "Return"},
	                                     {"Laufschrift modus", "Laufgeschwindigkeit", "Zurück"}};
	static char menuItem = 1;

	if (event == ON_ENTRY)
		menuItem = 1;

	else if (event & CHANGE) { // only react to input changes
		switch (event & INPUT_MASK) {
		case BTN_DOWN:
			if (--menuItem < 0)
				menuItem = 2;
			break;
		case BTN_UP:
			if (++menuItem > 2)
				menuItem = 0;
			break;
		case BTN_MENU:
			switch (menuItem) {
			case 0:
				TRANSITION(stateShiftMode);
				break;
			case 1:
				TRANSITION(stateShiftSpeed);
				break;
			case 2:
				TRANSITION(stateMainMenu);
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
	display->setText(menuText[language][menuItem]);
}

void NameTagSM::stateShiftMode(byte event){

	static const char* menuText[2][4] = {{"Automatik", "Shifting everytime", "NO Shifting","Return"},
	                                     {"Automatisch", "Immer Schieben", "Kein Schieben","Zurueck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		menuItem = 0;

	else if (event & CHANGE) { // only react to input changes
		switch (event & INPUT_MASK) {
		case BTN_DOWN:
			if (--menuItem < 0)
				menuItem = 3;
			break;
		case BTN_UP:
			if (++menuItem > 3)
				menuItem = 0;
			break;
		case BTN_MENU:
			switch (menuItem) {
			case 0:
				display->setShiftMode(display->AUTO_SHIFT);
				break;
			case 1:
				display->setShiftMode(display->SHIFT);
				break;
			case 2:
				display->setShiftMode(display->NO_SHIFT);
				break;
			case 3:
				TRANSITION(stateSettingsMenu);
			}
		default:
			return; // all buttons released
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setText(menuText[language][menuItem]);
}

void NameTagSM::stateShiftSpeed(byte event){
	static char menuItem = 0;

	if (event == ON_ENTRY)
		menuItem = display->shiftSpeed();

	else if (event & CHANGE) { // only react to input changes
		switch (event & INPUT_MASK) {
		case BTN_DOWN:
			if (--menuItem < 0)
				menuItem = 21;
			break;
		case BTN_UP:
			if (++menuItem > 21)
				menuItem = 0;
			break;
		case BTN_MENU:
			if (menuItem != 21) {
				display->setShiftSpeed(menuItem);
			}
			TRANSITION(stateSettingsMenu);
		default:
			return; // all buttons released
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	if (menuItem == 21) {
		display->setText(language == ENGLISH ? "Return" : "Zurück");
		return;
	} else {
		display->setText(display->formatInt(num_buffer, 10, menuItem));
	}
}

void NameTagSM::stateLanguageMenu(byte event){

	static const char* menuText[2][3] = {{"English", "German", "Return"},
	                                     {"English", "Deutsch", "Zurück"}};

	static char menuItem;

	if (event == ON_ENTRY)
		menuItem = language;

	else if (event & CHANGE) { // only react to input changes
		switch (event & INPUT_MASK) {
		case BTN_DOWN:
			if (--menuItem < 0)
				menuItem = 2;
			break;
		case BTN_UP:
			if (++menuItem > 2)
				menuItem = 0;
			break;
		case BTN_MENU:
			if (menuItem != 2)
				language = static_cast<Language>(menuItem);
			TRANSITION(stateMainMenu);
		default:
			return; // all buttons released
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setText(menuText[language][menuItem]);
}

void NameTagSM::stateDisplayMenu(byte event){

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
