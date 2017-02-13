#pragma once

#include "statemachine.h"

#define ENTER    bit(7)
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
	NameTag *display;
	const char** menuEntries;

	unsigned long time;
	const char* name_text;

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
};
