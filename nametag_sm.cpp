#include "nametag_sm.h"
#include "nametag.h"

#include "avr/eeprom.h"

#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&NameTagSM::s)); \
	process(ON_ENTRY); \
}

char EE_names[8][MAX_TEXT_LEN] EEMEM = {
   "1: Felix",
   "2: Tekkietorium - Felix Haschke & Fabian Umhang",
   "3: Fabian",
   "4: Milan",
   "5: Felix Haschke",
   "6: Wir bieten auch für dich dieses Namensschild an",
   "7: ",
   "8: "
};
byte EE_shift_speed EEMEM = 5;
byte EE_shift_mode EEMEM = NameTag::AUTO_SHIFT;
byte EE_language EEMEM = 0;
byte EE_selected EEMEM = 0;
byte EE_orientation EEMEM = 1;

#define MENU_ITEM_PREV    127
#define MENU_ITEM_DEFAULT 126

NameTagSM::NameTagSM(NameTag *display)
   : StateMachine(STATE_CAST(&NameTagSM::stateDefault))
   , display(display), language(ENGLISH), next_menu_item(MENU_ITEM_DEFAULT)
{
	// setup input pins
	DDRB &= ~INPUT_MASK;
	PORTB |= INPUT_MASK;

	display->setShiftSpeed(eeprom_read_byte(&EE_shift_speed));
	display->setShiftMode(static_cast<NameTag::ShiftMode>(eeprom_read_byte(&EE_shift_mode)));
	language = static_cast<Language>(eeprom_read_byte(&EE_language));
	display->setFlipped(eeprom_read_byte(&EE_orientation) == 1);

	selected_name = eeprom_read_byte(&EE_selected);
	eeprom_read_block(name_text, EE_names[selected_name], MAX_TEXT_LEN);
	display->setText(name_text + 3);
}

bool NameTagSM::advance(byte event, char& item, const char num, const char min) {
	switch (event & INPUT_MASK) {
	case BTN_DOWN:
		if (--item < min)
			item = num-1;
		break;
	case BTN_UP:
		if (++item >= num)
			item = min;
		break;
	case BTN_MENU:
		return true;
	}
	return false;
}

void NameTagSM::initMenuItem(char& item, const char _default, const char num) {
	if (next_menu_item == MENU_ITEM_PREV);
	else if (next_menu_item == MENU_ITEM_DEFAULT)
		item = _default;
	else if (next_menu_item < 0)
		item = num + next_menu_item;
	else
		item = next_menu_item;
	next_menu_item = MENU_ITEM_PREV;
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

	if (event == ON_ENTRY)
		initMenuItem(menuItem, 1, 7);
	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 7)) {
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
				TRANSITION(stateHelpMenu);
				break;
			case 6:
				TRANSITION(stateDefault);
				break;
			}
			return;
		}
	} else {
		display->update();
		return;
	}
	display->setText(menuText[language][menuItem]);
}

void NameTagSM::stateSettingsMenu(byte event){

	static const char* menuText[2][4] = {{"Shiftmode",
	                                      "Shiftspeed",
	                                      "Orientation",
	                                      "Return"
	                                     },
	                                     {"Laufschriftmodus",
	                                      "Laufgeschwindigkeit",
	                                      "Ausrichtung",
	                                      "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, 0, 4);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 4)) {
			switch (menuItem) {
			case 0:
				TRANSITION(stateShiftMode);
				break;
			case 1:
				TRANSITION(stateShiftSpeed);
				break;
			case 2:
				TRANSITION(stateOrientationMenu);
				break;
			case 3:
				next_menu_item = MENU_ITEM_PREV;
				TRANSITION(stateMainMenu);
				break;
			}
			return;
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setText(menuText[language][menuItem]);
}

void NameTagSM::stateShiftMode(byte event){

	static const char* menuText[2][3] = {{"auto", "always", "Return"},
	                                     {"auto", "immer", "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, display->shiftMode(), 3);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 3)) {
			switch (menuItem) {
			case 0:
				display->setShiftMode(NameTag::AUTO_SHIFT);
				break;
			case 1:
				display->setShiftMode(NameTag::SHIFT);
				break;
			case 2:
				TRANSITION(stateSettingsMenu);
				return;
			}
			eeprom_update_byte(&EE_shift_mode, display->shiftMode());
			return;
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setText(menuText[language][menuItem]);
}

void NameTagSM::stateShiftSpeed(byte event) {
	const char MAX_SPEED = 21;
	static char menuItem = 0;

	if (event == ON_ENTRY)
		menuItem = 21-display->shiftSpeed();

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, MAX_SPEED)) {
			if (menuItem != 0) {
				byte value = 21-menuItem;
				display->setShiftSpeed(value);
				eeprom_update_byte(&EE_shift_speed, value);
			}
			TRANSITION(stateSettingsMenu);
			return;
		}
		time = millis() + 500;
	} else if ((event & (BTN_DOWN | BTN_UP)) && millis() > time+250) {
		time = millis();
		advance(event, menuItem, MAX_SPEED);
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	if (menuItem == 0) {
		display->setText(language == ENGLISH ? "Return" : "Zur""\x1f""ck");
	} else {
		display->setText(display->formatInt(num_buffer, 10, menuItem));
	}
}

void NameTagSM::stateLanguageMenu(byte event){

	static const char* menuText[2][3] = {{"English", "German", "Return"},
	                                     {"English", "Deutsch", "Zur""\x1f""ck"}};

	static char menuItem = 0;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, language, 3);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 3)) {
			if (menuItem != 2){
				language = static_cast<Language>(menuItem);
				eeprom_update_byte(&EE_language, language);
			}
			TRANSITION(stateMainMenu);
			return;
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setText(menuText[language][menuItem]);
}

void NameTagSM::stateDisplayMenu(byte event){

	static const char* menuText[2][4] = {{"Change Name", "Create Name", "Delete Name", "Return"},
	                                     {"Name wechseln", "Name hinzuf""\x1f""gen", "Name löschen", "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, 0, 4);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 4)) {
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
				TRANSITION(stateMainMenu);
				break;
			}
			return;
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setText(menuText[language][menuItem]);
}

void NameTagSM::stateChangeName(byte event){
	static const char* menuText[2] = {"Return", "Zur""\x1f""ck"};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, selected_name, 9);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 9)) {
			if(menuItem != 8) {
				selected_name = menuItem;
				display->setText(name_text + 3);
				eeprom_update_byte(&EE_selected, menuItem);
			}
			TRANSITION(stateDisplayMenu);
			return;
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	if (menuItem == 8)
		display->setText(menuText[language]);
	else {
		// display new menu text
		eeprom_read_block(name_text, EE_names[menuItem], MAX_TEXT_LEN);
		display->setText(name_text);
	}
}

void NameTagSM::stateCreateName(byte event){

	static const char* menuText[2][7] =
	{{
	    "Special character",
	    "numbers",
	    "capital Letters",
	    "small Letters",
	    "edit previous letter",
	    "save name",
	    "Return"
	 },
	 {
	    "Sonderzeichen",
	    "Zahlen",
	    "Großbuchstaben",
	    "Kleinbuchstaben",
	    "vorheringen Buchstabe bearbeiten",
	    "Name Speichern",
	    "Zur""\x1f""ck"
	 }};
	/*
	static char menuItem = 0;
	static bool capitalization = 0;
	static const char* Name;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, 0, 31);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 3)) {
			if(menuItem >= 0 && menuItem < 26){
				//*Name = menuItem;
				//++Name;
			}
			else {
				switch (menuItem) {
				case 26:
					//TRANSITION(stateCreateSpecialCharacterMenu);
					break;
				case 27:
					//TRANSITION(stateCreateNumberMenu);
					break;
				case 28:
					if(capitalization == false)
						capitalization = true;
					else
						capitalization = false;
					break;
				case 29:
					--Name;
					break;
				case 30:
					display->setText(Name);
					return;
				case 31:
					next_menu_item = MENU_ITEM_PREV;
					TRANSITION(stateMainMenu);
					break;
				}
				return;
			}
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	if(menuItem >= 0 && menuItem < 26){
		display->setChar(menuItem,5);
	}
	else switch (menuItem) {
	case 26:
		display->setText(menuText[language][0]);
		break;
	case 27:
		display->setText(menuText[language][1]);
		break;
	case 28:
		if (capitalization == false)
			display->setText(menuText[language][2]);
		else
			display->setText(menuText[language][3]);
		break;
	case 29:
	case 30:
	case 31:
		display->setText(menuText[language][menuItem-25]);
		break;
	default:
		break;
	}
*/
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

	if (event == ON_ENTRY)
		initMenuItem(menuItem, 0, 3);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 3)) {
			switch (menuItem) {
			case 0:
				TRANSITION(stateDisplayTest);
				break;
			case 1:
				//TRANSITION(stateButtonTest);
				break;
			case 2:
				next_menu_item = true;
				TRANSITION(stateMainMenu);
				break;
			}
			return;
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setText(menuText[language][menuItem]);
}

void NameTagSM::stateDisplayTest(byte event){
	if (event == ON_ENTRY) {
		display->setText("\x29""\x30""\x31""0123456789");
	} else if (event & CHANGE && event & INPUT_MASK) { // only react to input changes
		TRANSITION(stateTestsMenu);
		return;
	} else { // no change
		display->update();
		return; // do not call setText()
	}
}

void NameTagSM::stateResetSettings(byte event){

	static const char* menuText[2][2] = {{"reset to factory settings ?",
	                                      "Return"},
	                                     {"Zur""\x1f""cksetzen auf Werkeinstellungen ?",
	                                      "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, 0, 2);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 2)) {
			switch (menuItem) {
			case 0:
				eeprom_update_byte(&EE_language, language = ENGLISH);
				eeprom_update_byte(&EE_selected, selected_name = 0);
				display->setShiftMode(NameTag::AUTO_SHIFT);
				eeprom_update_byte(&EE_shift_mode, display->shiftMode());
				display->setShiftSpeed(5);
				eeprom_update_byte(&EE_shift_speed, display->shiftSpeed());
				TRANSITION(stateMainMenu);
				break;
			case 1:
				TRANSITION(stateMainMenu);
				break;
			}
			return;
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setText(menuText[language][menuItem]);

}

void NameTagSM::stateHelpMenu(byte event){

	static const char* menuText[2][3] = {{"www.tekkietorium.de",
	                                      "tekkietorim@gmx.de",
	                                      "more info: www.tekkietorium.de/Projekte/LEDNamensschild",
	                                     },
	                                     {"www.tekkietorium.de",
	                                      "tekkietorium@gmx.de",
	                                      "mehr Infos: wwww.tekkietorium.de(Projekte/LEDNamensschild"
	                                     }};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, 0, 3);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (event & BTN_MENU){
			TRANSITION(stateMainMenu);
		}
		advance(event, menuItem, 3);
		return;
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setText(menuText[language][menuItem]);
}
void NameTagSM::stateOrientationMenu(byte event){

	static const char* menuText[2][3] = {{"bottom",
	                                      "top",
	                                      "return"
	                                     },
	                                     {"unten",
	                                      "oben",
	                                      "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, display->flipped(), 3);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 3)) {
			switch (menuItem) {
			case 0:
			case 1:
				eeprom_update_byte(&EE_orientation, menuItem);
				display->setFlipped(menuItem == 1);
				// do not break here
			case 2:
				TRANSITION(stateSettingsMenu);
			}
			return;
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}

	// display new menu text
	display->setText(menuText[language][menuItem]);
}
