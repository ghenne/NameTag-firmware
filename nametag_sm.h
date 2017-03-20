#pragma once

#include "statemachine.h"
#define ON_ENTRY bit(7)
#define CHANGE   bit(0)
// these are the actual input pins (of PINC)
#define BTN_DOWN bit(1)
#define BTN_MENU bit(2)
#define BTN_UP   bit(3)
// mask of all input pins
#define INPUT_MASK (BTN_DOWN | BTN_UP | BTN_MENU)

#define MAX_NAME_LEN 64
#define MAX_TEXT_LEN (MAX_NAME_LEN + 4)

class NameTag;
class NameTagSM : public StateMachine
{
	enum Language {ENGLISH = 0, DEUTSCH = 1};

	NameTag *display;
	Language language;
	unsigned long time;
	char name_text[MAX_TEXT_LEN];
	char num_buffer[10];
	int selected_name;
	char next_menu_item;

public:
	NameTagSM(NameTag *display);

private:
	/// increment / decrement item based on UP/DOWN event
	/// return true iff BTN_MENU was pressed
	bool advance(byte event, char &item, const char max, const char min=0);
	/// initialize menu item to next_menu_item or _default
	void initMenuItem(char &item, const char _default, const char num);

	void stateDefault(byte event);
	void stateEnterMenu(byte event);
	void stateMainMenu(byte event);
	void stateLanguageMenu(byte event);
	void stateSettingsMenu(byte event);
	void stateShiftMode(byte event);
	void stateShiftSpeed(byte event);
	void stateDisplayMenu(byte event);
	void stateChangeName(byte event);
	void stateCreateName(byte event);
	void stateDeleteName(byte event);
	void stateTestsMenu(byte event);
	void stateResetSettings(byte event);
	void stateHelpMenu(byte event);
	void stateDisplayTest(byte event);
};
