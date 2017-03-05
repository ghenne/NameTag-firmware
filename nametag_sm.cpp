#include "nametag_sm.h"
#include "nametag.h"

#include "avr/eeprom.h"

#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&NameTagSM::s)); \
	process(ON_ENTRY); \
}

byte EE_shift_speed EEMEM = 5;
byte EE_shift_mode EEMEM = NameTag::AUTO_SHIFT;
byte EE_language EEMEM = 0;
byte EE_selected EEMEM = 0;
char EE_names[8][MAX_TEXT_LEN] EEMEM = {
   "1: Felix",
   "2: Tekkietorium - Felix Haschke & Fabian Umhang",
   "3: Fabian",
   "4: Milan",
   "5: ",
   "6: ",
   "7: ",
   "8: "
};

NameTagSM::NameTagSM(NameTag *display)
   : StateMachine(STATE_CAST(&NameTagSM::stateDefault))
   , display(display), language(ENGLISH)
{
	// setup input pins
	DDRC &= ~INPUT_MASK;
	PORTC |= INPUT_MASK;

	selected_name = eeprom_read_byte(&EE_selected);
	eeprom_read_block(name_text, EE_names[selected_name], MAX_TEXT_LEN);
	display->setText(name_text + 3);

	display->setShiftSpeed(eeprom_read_byte(&EE_shift_speed));
	display->setShiftMode(static_cast<NameTag::ShiftMode>(eeprom_read_byte(&EE_shift_mode)));
	language = static_cast<Language>(eeprom_read_byte(&EE_language));
}

void NameTagSM::stateDefault(byte event) // default state: print name
{
	if (event == ON_ENTRY) {
		display->setText(name_text + 3);
		return;
	}

	if (event & BTN_MENU){
		TRANSITION(stateEnterMenu)
	}
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
		//TRANSITION(stateDefault);
	}
}

void NameTagSM::stateMainMenu(byte event)
{
	static const char* menuText[2][7] = {{"Language",
	                                      "Display",
	                                      "Settings",
	                                      "Reset to Factory Settings",
	                                      "Tests",
	                                      "help & system Information",
	                                      "Return"
	                                     },
	                                     {"Sprache",
	                                      "Anzeige",
	                                      "Einstellungen",
	                                      "Zur""\x1f""cksetzen auf Werkseinstellungen",
	                                      "Tests",
	                                      "Hilfe und System Informationen",
	                                      "Zur""\x1f""ck"
	                                     }};
	static char menuItem = 0;

	if (event == ON_ENTRY){
		menuItem = 1;
	}
	else if (next_menuItem == true){
		menuItem = 6;
		next_menuItem = false;
	}
	else if (event & CHANGE) { // only react to input changes
		switch (event & INPUT_MASK) {
		case BTN_DOWN:
			if (--menuItem < 0)
				menuItem = 6;
			break;
		case BTN_UP:
			if (++menuItem > 6)
				menuItem = 0;
			break;
		case BTN_MENU:
			switch (menuItem) {
			case 0:
				TRANSITION(stateLanguageMenu);
				break;
			case 1:
				TRANSITION(stateDisplayMenu);
				break;
			case 2:
				TRANSITION(stateSettingsMenu);
				break;
			case 3:
				TRANSITION(stateResetSettings);
				break;
			case 4:
				TRANSITION(stateTestsMenu);
				break;
			case 5:
				//TRANSITION(stateHelpMenu);
				break;
			case 6:
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
	                                     {"Laufschrift modus", "Laufgeschwindigkeit", "Zur""\x1f""ck"}};
	static char menuItem = 1;

	if (event == ON_ENTRY){
		menuItem = 0;
	}
	else if (next_menuItem == true){
		menuItem = 2;
		next_menuItem = false;
	}

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
				next_menuItem = true;
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

	static const char* menuText[2][4] = {{"Automatik", "Shifting everytime", "Return"},
	                                     {"Automatisch", "Immer Schieben", "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		menuItem = display->shiftMode();
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
				//eeprom_update_byte(&shift_mode_ee,display->AUTO_SHIFT);
				break;
			case 1:
				display->setShiftMode(display->SHIFT);
				//eeprom_update_byte(&shift_mode_ee,display->SHIFT);
				break;
			case 2:
				next_menuItem = true;
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
	static char menuItem = 1;

	if (event == ON_ENTRY)
		menuItem = display->shiftSpeed();

	else if (event & CHANGE) { // only react to input changes
		switch (event & INPUT_MASK) {
		case BTN_DOWN:
			if (--menuItem < 0)
				menuItem = 20;
			break;
		case BTN_UP:
			if (++menuItem > 20)
				menuItem = 0;
			break;
		case BTN_MENU:
			if (menuItem != 0) {
				display->setShiftSpeed(menuItem);
				eeprom_update_byte(&EE_shift_speed, menuItem);
			}
			next_menuItem = true;
			TRANSITION(stateSettingsMenu);
		default:
			return; // all buttons released
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	if (menuItem == 0) {
		display->setText(language == ENGLISH ? "Return" : "Zur""\x1f""ck");
		return;
	} else {
		display->setText(display->formatInt(num_buffer, 10, menuItem));
	}
}

void NameTagSM::stateLanguageMenu(byte event){

	static const char* menuText[2][3] = {{"English", "German", "Return"},
	                                     {"English", "Deutsch", "Zur""\x1f""ck"}};

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
			if (menuItem != 2){
				language = static_cast<Language>(menuItem);
				eeprom_update_byte(&EE_language, language);
			}
			next_menuItem = true;
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

	static const char* menuText[2][4] = {{"Change Name", "Create Name", "Delete Name","Return"},
	                                     {"Name wechseln", "Name hinzufuegen", "Name löschen", "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY){
		menuItem = 0;
	}
	else if (next_menuItem == true){
		menuItem = 3;
		next_menuItem = false;
	}

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
				TRANSITION(stateChangeName);
				break;
			case 1:
				//TRANSITION(stateCreateName);
				break;
			case 2:
				//TRANSITION(stateDeleteName);
				break;
			case 3:
				next_menuItem = true;
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

void NameTagSM::stateChangeName(byte event){
	static const char* menuText[2][2] = {{"Return"},
	                                     {"Zur""\x1f""ck"}};
	static char menuItem = 0;//eeprom_read_byte(&selected_ee);

	if (event == ON_ENTRY)
		menuItem = 0;
	else if (event & CHANGE) { // only react to input changes
		switch (event & INPUT_MASK) {
		case BTN_DOWN:
			if (--menuItem < 0)
				menuItem = 8;
			break;
		case BTN_UP:
			if (++menuItem > 8)
				menuItem = 0;
			break;
		case BTN_MENU:
			if(menuItem != 8) {
				selected_name = menuItem;
				display->setText(name_text + 3);
				eeprom_update_byte(&EE_selected, menuItem);
			}
			next_menuItem = true;
			TRANSITION(stateDisplayMenu);
			return;
		default:
			return; // all buttons released
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}
	;
	// display new menu text
	if (menuItem == 8) {
		display->setText(menuText[language][0]);
	}
	else {
		// display new menu text
		eeprom_read_block(name_text, EE_names[menuItem], MAX_TEXT_LEN);
		display->setText(name_text);
	}
}
void NameTagSM::stateCreateName(byte event){

}

void NameTagSM::stateDeleteName(byte event){

}

void NameTagSM::stateTestsMenu(byte event){

	static const char* menuText[2][3] = {{"Test the Display",
	                                      "Test the Buttons",
	                                      "Return"
	                                     },
	                                     {"Test des Displays",
	                                      "Test der Taster",
	                                      "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY){
		menuItem = 0;
	}
	else if (next_menuItem == true){
		menuItem = 2;
		next_menuItem = false;
	}

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
				TRANSITION(stateDisplayTest);
				break;
			case 1:
				//TRANSITION(stateButtonTest);
				break;
			case 2:
				next_menuItem = true;
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

void NameTagSM::stateDisplayTest(byte event){

	static const char* menuText[2][3] = {{"Test the Display",
	                                      "Test the Buttons",
	                                      "Return"
	                                     },
	                                     {"Test des Displays",
	                                      "Test der Taster",
	                                      "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY){
		menuItem = 0;
	}
	else if (next_menuItem == true){
		menuItem = 2;
		next_menuItem = false;
	}

	else if (event & CHANGE) { // only react to input changes
		switch (event & INPUT_MASK) {
		case BTN_DOWN:
		case BTN_UP:
		case BTN_MENU:
			TRANSITION(stateTestsMenu);
		default:
			return; // all buttons released
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setChar(200,0);
	display->setChar(200,4);
	display->setChar(200,8);
	display->setChar(200,12);
	display->setChar(200,16);
	display->setChar(200,20);
	display->setChar(200,24);
	display->setChar(200,28);
	display->setChar(200,32);
	display->setChar(200,36);
}




void NameTagSM::stateResetSettings(byte event){

	static const char* menuText[2][2] = {{"reset to factory settings ?",
	                                      "Return"},
	                                     {"Zur""\x1f""cksetzen auf Werkeinstellungen ?",
	                                      "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		menuItem = display->shiftMode();
	else if (event & CHANGE) { // only react to input changes
		switch (event & INPUT_MASK) {
		case BTN_DOWN:
			if (--menuItem < 0)
				menuItem = 1;
			break;
		case BTN_UP:
			if (++menuItem > 1)
				menuItem = 0;
			break;
		case BTN_MENU:
			switch (menuItem) {
			case 0:
				eeprom_update_byte(&EE_language, language = ENGLISH);
				eeprom_update_byte(&EE_selected, selected_name = 0);
				display->setShiftMode(NameTag::AUTO_SHIFT);
				eeprom_update_byte(&EE_shift_mode, display->shiftMode());
				display->setShiftSpeed(5);
				eeprom_update_byte(&EE_shift_speed, display->shiftSpeed());
				next_menuItem = true;
				TRANSITION(stateMainMenu);
				break;
			case 1:
				next_menuItem = true;
				TRANSITION(stateMainMenu);
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

void NameTagSM::statehelpMenu(byte event){

}
