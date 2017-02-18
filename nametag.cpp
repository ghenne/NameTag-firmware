#include "nametag.h"

NameTag::NameTag(byte num)
   : MatrixDisplay(num)
   , start_col_(0), shift_mode_(AUTO_SHIFT), shift_count_(0), shift_speed_(10)
{
}

// shift text, depending on shift_mode_ and shift_speed_
void NameTag::update() {
	if (shift_mode_ & SHIFT) {
		if (++shift_count_ == shift_speed_) {
			shift_count_ = 0;
			shift();
		}
	}
}

// shift text by one column and start over if nothing is shown anymore
void NameTag::shift()
{
	--start_col_;
	if (setString(text_, start_col_) <= 0)
		start_col_ = 7;
}

// display (and remember) text (for future shifting)
void NameTag::setText(const char* text)
{
	text_ = text;

	clear();
	// need to recompute actual shift mode?
	if (shift_mode_ & AUTO_SHIFT)
		setShiftMode(AUTO_SHIFT);
}

void NameTag::setShiftMode(NameTag::ShiftMode mode)
{
	shift_mode_ = mode;
	if (shift_mode_ & AUTO_SHIFT) {        // if the shift mode is auto,
		bool shift = width(text_) > cols(); // determined if shifting is needed
		bitWrite(shift_mode_, 0, shift); // set the second bit of shift_mode_ to
		                                 // previously calculated state
	}
	if (shift_mode_ & SHIFT) // when shifting, start is last column
		start_col_ = cols()-1;
	else // otherwise, start in first column + setString once
		setString(text_, start_col_ = 0);
}

void NameTag::setShiftSpeed(byte speed) {
	shift_speed_ = speed;
}
