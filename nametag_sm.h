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

class NameTag;
class NameTagSM : public StateMachine
{
	enum Language {ENGLISH = 0, DEUTSCH = 1};

	NameTag *display;
	Language language;
	unsigned long time;
	const char* name_text;
	char num_buffer[10];

public:
	NameTagSM(NameTag *display);

private:
#if 0
	void eepromSetName (const char* text, byte number);
	const char* eepromReadName(byte number);
#endif

	void stateDefault(byte event);
	void stateEnterMenu(byte event);
	void stateMainMenu(byte event);
	void stateLanguageMenu(byte event);
	void stateDisplayMenu(byte event);
	void stateSettingsMenu(byte event);
	void stateShiftMode(byte event);
	void stateShiftSpeed(byte event);
};
