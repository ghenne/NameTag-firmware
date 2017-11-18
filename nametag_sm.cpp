#include "nametag_sm.h"
#include "nametag.h"

#include "avr/eeprom.h"
#include "names.h"

#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&NameTagSM::s)); \
	process(ON_ENTRY); \
}

char EE_names[8][MAX_TEXT_LEN] EEMEM = {
   "1: " NAME1,
   "2: " NAME2,
   "3: " NAME3,
   "4: " NAME4,
   "5: " NAME5,
   "6: " NAME6,
   "7: " NAME7,
   "8: " NAME8
};
byte EE_shift_speed EEMEM = 15;
byte EE_shift_mode EEMEM = NameTag::AUTO_SHIFT;
byte EE_language EEMEM = NameTagSM::default_language;
byte EE_selected EEMEM = 0;
byte EE_orientation EEMEM = 1;

#define MENU_ITEM_PREV    127
#define MENU_ITEM_DEFAULT 126

NameTagSM::NameTagSM(NameTag *display)
   : StateMachine(STATE_CAST(&NameTagSM::stateDefault))
   , display(display), language(default_language), next_menu_item(MENU_ITEM_DEFAULT)
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
	static const char* menuText[2][6] = {{ "name",
	                                      "settings",
	                                      "reset",
	                                      "tests",
	                                      "information",
	                                      "return"
	                                     },
	                                     {"Name",
	                                      "Einstellungen",
	                                      "Zur""\x1f""cksetzen",
	                                      "Tests",
	                                      "Informationen",
	                                      "Zur""\x1f""ck"
	                                     }};
	static char menuItem = 0;
	if (event == ON_ENTRY)
		initMenuItem(menuItem, 1, 6);
	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 6)) {
			switch (menuItem) {
			case 0:
				TRANSITION(stateDisplayMenu);
				break;
			case 1:
				TRANSITION(stateSettingsMenu);
				break;
			case 2:
				TRANSITION(stateResetSettings);
				break;
			case 3:
				TRANSITION(stateTestsMenu);
				break;
			case 4:
				TRANSITION(stateHelpMenu);
				break;
			case 5:
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

	static const char* menuText[2][5] = {{"language",
	                                      "shiftmode",
	                                      "shiftspeed",
	                                      "orientation",
	                                      "return"
	                                     },
	                                     {"Sprache",
	                                      "Laufschriftmodus",
	                                      "Laufgeschwindigkeit",
	                                      "Ausrichtung",
	                                      "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, 0, 5);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 5)) {
			switch (menuItem) {
			case 0:
				TRANSITION(stateLanguageMenu);
				break;
			case 1:
				TRANSITION(stateShiftMode);
				break;
			case 2:
				TRANSITION(stateShiftSpeed);
				break;
			case 3:
				TRANSITION(stateOrientationMenu);
				break;
			case 4:
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

	static const char* menuText[2][3] = {{"auto", "always", "return"},
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
		display->setText(language == ENGLISH ? "return" : "Zur""\x1f""ck");
	} else {
		display->setText(display->formatInt(num_buffer, 10, menuItem));
	}
}

void NameTagSM::stateLanguageMenu(byte event){

	static const char* menuText[2][3] = {{"English", "German", "return"},
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

// select name for editing / display
void NameTagSM::stateDisplayMenu(byte event){
	static const char* menuText[2] = {"return", "Zur""\x1f""ck"};
	static char menuItem = 0;

	if (event == ON_ENTRY) {
		edited_name = selected_name;
		initMenuItem(menuItem, edited_name, 9);
	}
	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 9)) {
			if(menuItem != 8) {
				edited_name = menuItem;
				TRANSITION(stateEditOptionsMenu);
			}
			else
				TRANSITION(stateMainMenu);
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
void NameTagSM::stateEditOptionsMenu(byte event){
	static const char* menuText[2][4] = {{"select name", "edit name", "delete Name", "return"},
	                                     {"Name ausw""\x1d""hlen", "Name bearbeiten", "Name l""\x1e""schen", "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, 1, 4);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 4)) {
			switch (menuItem) {
			case 0:
				selected_name = edited_name;
				eeprom_update_byte(&EE_selected, selected_name);
				++menuItem; // switch to next menu item
				break;
			case 1:
				display->setCursor(0);
				TRANSITION(stateEditSelectChar);
				return;
			case 2:
				name_text[3] = '\0';
				eeprom_update_block(name_text, EE_names[edited_name], MAX_TEXT_LEN);
				--menuItem; // switch to previous menu item
				break;
			case 3:
				menuItem = 0;
				TRANSITION(stateDisplayMenu);
				return;
			}
		}
	} else { // no change
		display->update();
		return;
	}
	// display new menu text
	display->setText(menuText[language][menuItem]);
}

void NameTagSM::adjustFirstVisible(byte &first, char cursor)
{
	if (cursor <= first)
		first = cursor;
	else {
		int column = 0;
		byte pos = first;
		// find column after cursor
		for (char *pc = name_text + first; *pc; ++pc, ++pos) {
			column += display->width(*pc) + 1;
			if (pos == cursor)
				break;
		}
		// shift chars until cursor becomes visible
		char *pc = name_text + first;
		while (column >= display->cols()) {
			column -= display->width(*pc) + 1;
			++pc; ++first;
		}
	}
}

// select char for editing
void NameTagSM::stateEditSelectChar(byte event) {
	static byte button_state; // remember previous button state

	byte old_button_state = button_state;
	button_state = event & INPUT_MASK;

	if (event == ON_ENTRY) {
		old_button_state = button_state = (~PINB & INPUT_MASK);
		edit_first_visible = 3;
		adjustFirstVisible(edit_first_visible, display->getCursor()+3);
		time = 0;
	}
	byte changes = old_button_state ^ button_state;

	if (button_state < old_button_state) {
		// any button released
		if (changes & BTN_MENU && time != 0) {
			// menu button released (after first press within this state, i.e. time != 0)
			TRANSITION(stateEditChar);
			return;
		}
		time = 0; // reset time
	} else if (changes & BTN_MENU && button_state & BTN_MENU) {
		// menu button pressed
		time = millis() + 500;
	} else if (button_state & BTN_MENU) {
		// menu button still pressed
		if (time != 0 && millis() > time) {
			// save current name
			eeprom_update_block(name_text, EE_names[edited_name], MAX_TEXT_LEN);
			display->setCursor(-1);
			TRANSITION(stateEditOptionsMenu);
			return;
		}
	}

	// if up/down pressed, advance char
	if (button_state & (BTN_DOWN | BTN_UP) &&
	        (time == 0 || millis() > time)) {
		// remember last update time
		time = millis() + (time == 0 ? 500 : 100);

		char cursor = display->getCursor();
		int len = strlen(name_text+3);
		// allow cursor to be after text (to extend)
		advance(button_state, cursor, len + (len < MAX_NAME_LEN ? 1 : 0), 0);
		if (cursor+2 >= len) {
			char *pc = name_text+3+len; // pointer to last char (\0) of name_text
			if (cursor == len)
				*pc++ = ' '; // extend by one space;
			else if (cursor == len-2 && pc[-1] == ' ')
				--pc; // shorten text
			*pc = '\0'; // define new end of string
		}
		display->setCursor(cursor);

		// adjust edit_first_visible, such that cursor becomes visible
		adjustFirstVisible(edit_first_visible, cursor+3);
	}

	int last = display->setString(name_text + edit_first_visible, 0,
	                              display->getCursor() + 3 - edit_first_visible, 1);
	display->clearColumns(last, display->cols());
}

void NameTagSM::stateEditChar(byte event){
	static char sel_char = 0;

	if (event == ON_ENTRY) {
		time = 0;
		//sel_char = name_text[3+display->getCursor()] - ' ';
	}

	else if (event & CHANGE && event & BTN_MENU) {
		// char accepted
		TRANSITION(stateEditSelectChar);
		return;
	}

	unsigned long now = millis();
	byte dir = event & (BTN_DOWN | BTN_UP);
	if (!dir)
		time = 0; // reset time when both buttons released

	// if dir button first pressed (time==0) or still pressed
	if (dir && (time == 0 || now > time)) {
		// remember last update time
		time = now + (time == 0 ? 500 : 100);
		advance(event, sel_char, 100/*127-32*/, 0);
		//name_text[3+display->getCursor()] = ' ' + sel_char;
		if(sel_char == 0)										// Letter	A
			name_text[3+display->getCursor()] = 'A';
		else if(sel_char == 1)									// Letters	Ä
			name_text[3+display->getCursor()] = '\x1b';
		else if(sel_char >= 2 && sel_char <= 15)				// Letters	B-O
			name_text[3+display->getCursor()] = 'B' + sel_char  - 2;
		else if(sel_char == 16)							// Letters	Ö
			name_text[3+display->getCursor()] = '\x1c';
		else if(sel_char >= 17 && sel_char <= 22)				// Letters	P-U
			name_text[3+display->getCursor()] = 'P' + sel_char - 17;
		else if(sel_char == 23 || sel_char == 52)				// Letters	Ü & ü
			name_text[3+display->getCursor()] = '\x1f';
		else if(sel_char >= 24 && sel_char <= 28)				// Letters	V-Z
			name_text[3+display->getCursor()] = 'V' + sel_char - 24;
		else if(sel_char == 29)									// Letter	a
			name_text[3+display->getCursor()] = 'a';
		else if(sel_char == 30)									// Letter	a
			name_text[3+display->getCursor()] = '\x1d';
		else if(sel_char >= 31 && sel_char <= 44)				// Letter	b-o
			name_text[3+display->getCursor()] = 'b' + sel_char - 31;
		else if(sel_char == 45)									// Letter	ö
			name_text[3+display->getCursor()] = '\x1e';
		else if(sel_char >= 46 && sel_char <= 51)				// Letter	p-u
			name_text[3+display->getCursor()] = 'p' + sel_char - 46;
		else if(sel_char >= 51 && sel_char <= 57)				// Letter	v-z
			name_text[3+display->getCursor()] = 'v' + sel_char - 53;
		else if(sel_char == 58)									// Space
			name_text[3+display->getCursor()] = ' ';
		else if(sel_char >= 59 && sel_char <= 69)				// Nubers	0-9
			name_text[3+display->getCursor()] = '0' + sel_char - 59;
		else if(sel_char >= 70 && sel_char <= 85)				// characters	!,",#,$,%,&,',(,),*,+,,,-,-,\;
			name_text[3+display->getCursor()] = '!' + sel_char - 70;
		else if(sel_char >= 86 && sel_char <= 93)				// characters	:,;,<,=,>,?,@
			name_text[3+display->getCursor()] = ':' + sel_char - 86;
		else if(sel_char >= 93 && sel_char <= 96)				// characters	[, \, ]
			name_text[3+display->getCursor()] = '[' + sel_char - 93;
		else if(sel_char >= 96 && sel_char <= 100)				// characters	{, |, }, ~
			name_text[3+display->getCursor()] = '{' + sel_char - 96;

		else {
			name_text[3+display->getCursor()] = '?';
		}
	}

	char cursor = display->getCursor() + 3 - edit_first_visible;
	if (time != 0 || now % 1000 > 500)
		cursor = -1;
	int last = display->setString(name_text + edit_first_visible, 0, cursor, 1);
	display->clearColumns(last, display->cols());
}

void NameTagSM::stateTestsMenu(byte event){
	static const char* menuText[2][3] = {{"display test",
	                                      "button test",
	                                      "return"
	                                     },
	                                     {"Displaytest",
	                                      "Tastertest",
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
				TRANSITION(stateButtonTest);
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
	static byte menu_item = 0;
	if (event == ON_ENTRY) {
		menu_item = 0;
		display->setText("");
		display->clear();
		for(int i = 0; i < 32; i++){
			display->setColumn(i, 255);
		}
	} else if (event & CHANGE && event & INPUT_MASK) { // only react to input changes
		++menu_item;
		if(menu_item == 1)
			display->setText("\x29""\x30""\x31""0123456789");
		else {
			TRANSITION(stateTestsMenu);
			return;
		}
	} else { // no change
		display->update();
		return; // do not call setText()
	}
}

void NameTagSM::stateButtonTest(byte event){
	if (event == ON_ENTRY) {
		display->setText("");
		display->clear();
	} else if (event & CHANGE) { // only react to input changes
		for(int i = 24; i < 32; i++)
			display->setColumn(i, (event & BTN_UP ? 255 : 0));
		for(int i = 16; i < 24; i++)
			display->setColumn(i, (event & BTN_MENU ? 255 : 0));
		for(int i = 8; i < 16; i++)
			display->setColumn(i, (event & BTN_DOWN ? 255 : 0));
	} else { // no change
		display->update();
		return; // do not call setText()
	}
}


void NameTagSM::stateResetSettings(byte event){
	static const char* menuText[2][2] = {{"reset to factory settings ?",
	                                      "return"},
	                                     {"Zur""\x1f""cksetzen auf Werkseinstellungen ?",
	                                      "Zur""\x1f""ck"}};
	static char menuItem = 0;

	if (event == ON_ENTRY)
		initMenuItem(menuItem, 0, 2);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		if (advance(event, menuItem, 2)) {
			switch (menuItem) {
			case 0:
				eeprom_update_byte(&EE_language, language = NameTagSM::default_language);
				eeprom_update_byte(&EE_selected, selected_name = 0);
				display->setShiftMode(NameTag::AUTO_SHIFT);
				eeprom_update_byte(&EE_shift_mode, display->shiftMode());
				display->setShiftSpeed(5);
				eeprom_update_byte(&EE_shift_speed, display->shiftSpeed());
				eeprom_update_block(NAME1, EE_names[0]+3, MAX_NAME_LEN);
				eeprom_update_block(NAME2, EE_names[1]+3, MAX_NAME_LEN);
				eeprom_update_block(NAME3, EE_names[2]+3, MAX_NAME_LEN);
				eeprom_update_block(NAME4, EE_names[3]+3, MAX_NAME_LEN);
				eeprom_update_block(NAME5, EE_names[4]+3, MAX_NAME_LEN);
				eeprom_update_block(NAME6, EE_names[5]+3, MAX_NAME_LEN);
				eeprom_update_block(NAME7, EE_names[6]+3, MAX_NAME_LEN);
				eeprom_update_block(NAME8, EE_names[7]+3, MAX_NAME_LEN);
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
	                                       "return"
	                                     },
	                                     {"www.tekkietorium.de",
	                                      "tekkietorium@gmx.de",
	                                       "Zur""\x1f""ck"
	                                     }};
	static char menuItem = 0;

	 if (event == ON_ENTRY)
		initMenuItem(menuItem, 0, 3);

	else if (event & CHANGE && event & INPUT_MASK) { // only react to button presses
		  if (advance(event, menuItem, 3)) {
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
