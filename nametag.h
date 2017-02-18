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

private:
	const char* text_;
	char start_col_;
	byte shift_mode_;
	byte shift_count_ = 0;
	byte shift_speed_ = 10;
};
