#pragma once

#include "statemachine.h"
#define ON_ENTRY bit(7)
#define CHANGE   bit(3)
// these are the actual input pins (of PINC)
#define BTN_DOWN bit(0)
#define BTN_MENU bit(1)
#define BTN_UP   bit(2)
// mask of all input pins
#define INPUT_MASK (BTN_DOWN | BTN_UP | BTN_MENU)

#define MAX_NAME_LEN 64
#define MAX_TEXT_LEN (MAX_NAME_LEN + 4)

class NameTag;
class NameTagSM : public StateMachine
{
public:
	enum Language {ENGLISH = 0, DEUTSCH = 1};

	NameTagSM(NameTag *display);

private:
	/// increment / decrement item based on UP/DOWN event
	/// return true iff BTN_MENU was pressed
	bool advance(byte event, char &item, const char max, const char min=0);
	/// initialize menu item to next_menu_item or _default
	void initMenuItem(char &item, const char _default, const char num);
	void adjustFirstVisible(byte &first, char cursor);

	void stateDefault(byte event);
	void stateEnterMenu(byte event);
	void stateMainMenu(byte event);
	void stateLanguageMenu(byte event);
	void stateSettingsMenu(byte event);
	void stateShiftMode(byte event);
	void stateShiftSpeed(byte event);
	void stateDisplayMenu(byte event);
	void stateButtonTest(byte event);
	void stateEditOptionsMenu(byte event);
	void stateEditSelectChar(byte event);
	void stateEditChar(byte event);
	void stateCreateName(byte event);
	void stateDeleteName(byte event);
	void stateTestsMenu(byte event);
	void stateResetSettings(byte event);
	void stateHelpMenu(byte event);
	void stateDisplayTest(byte event);
	void stateOrientationMenu(byte event);

private:
	NameTag *display;
	Language language;
	unsigned long time;
	char name_text[MAX_TEXT_LEN];
	char num_buffer[10];
	byte selected_name, edited_name;
	byte edit_first_visible;
	char next_menu_item;
};
