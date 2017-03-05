#pragma once

#include "matrixdisplay.h"

class NameTag : public MatrixDisplay
{
public:
	enum ShiftMode { NO_SHIFT = 0,
		              SHIFT = 1,
		              AUTO_SHIFT = 2
	               };

	NameTag(byte num=1);
	void setText(const char* text);
	void update();

	void shift();
	void setShiftMode(ShiftMode mode);
	void setShiftSpeed(byte speed);

	byte shiftSpeed() const { return shift_speed_; }
	byte shiftMode();

private:
	const char* text_;  // displayed text
	const char* first_; // first visible character
	int  start_col_;
	byte shift_mode_;
	byte shift_count_;
	byte shift_speed_;
};
