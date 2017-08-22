#include "nametag.h"

NameTag::NameTag(byte num)
    : MatrixDisplay(num)
    , start_col_(0), shift_mode_(AUTO_SHIFT), shift_count_(0), curser_pos_(-1)
{
	setShiftSpeed(5);
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
	if (setString(first_, start_col_, curser_pos_ + text_-first_) <= 0) {
		first_ = text_;
		start_col_ = 7;
	} else { // should we advance to next start char?
		byte w = width(*first_);
		if (start_col_ + w < 0) {
			++first_;
			start_col_ += w+1; // char width + 1 column spacing
		}
	}
}

// display (and remember) text (for future shifting)
void NameTag::setText(const char* text)
{
	first_ = text_ = text;

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
	if (shift_mode_ & SHIFT) // when shifting, start in column 7
		start_col_ = 7;
	else // otherwise, start in first column + setString once
		setString(text_, start_col_ = 0, curser_pos_);
}

void NameTag::setShiftSpeed(byte speed) {
	shift_speed_ = speed << 3;
}

byte NameTag::shiftMode(){
	if(shift_mode_ & AUTO_SHIFT)
		return 2;
	return shift_mode_;
}

void NameTag::setCursor(char pos)
{
	curser_pos_ = pos;
}
