#pragma once

#include "statemachine.h"

#define BTN_DOWN bit(1)
#define BTN_MENU bit(2)
#define BTN_UP   bit(3)
#define INPUT_MASK (BTN_DOWN | BTN_UP | BTN_MENU)

class NameTag;
class NameTagSM : public StateMachine
{
	NameTag *display;
	unsigned long time;

public:
	NameTagSM(NameTag *display);

private:
	const char* name_text;
	void stateDefault(byte event);
	void stateEnterMenu(byte event);
	void stateMenu(byte event);
};
