#pragma once
#include "avr.h"

class StateMachine
{
public:
	typedef void (StateMachine::*State)(byte);
	StateMachine(State initial) : current_state(initial) {}

	void process(byte event) {
		(this->*current_state)(event);
	}
	void setState(State state) {
		current_state = state;
	}

private:
	State current_state;
};

#define STATE_CAST(s) reinterpret_cast<StateMachine::State>(s)
#define TRANSITION(s) setState(STATE_CAST(s))
