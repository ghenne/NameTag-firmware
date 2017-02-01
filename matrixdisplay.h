#pragma once
#include "avr.h"

class MatrixDisplay
{
public:
	MatrixDisplay(byte num=1);
	~MatrixDisplay();
	byte num() const { return num_; }
	void show(); // show current content

	void clearColumns(byte start, byte end);
	void setPixel(byte row, byte column, byte value);
	void setChar(unsigned char ch, byte column, byte row);

protected:
	byte *columnPtr(byte column) const;

private:
	const byte data_pin  = 0; // data for Shift register 74HC595 on PORTB 0
	const byte clock_pin = 1; // SHCP / clock pin for shift register on PORTB 1
	const byte latch_pin = 2; // STCP / latch pin for shift register on PORTB 2

	byte num_; // number of chained matrix displays
	byte *columns_; // display content of all matrices
};
