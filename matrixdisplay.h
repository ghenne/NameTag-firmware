#pragma once
#include "avr.h"

class MatrixDisplay
{
public:
	MatrixDisplay(byte num=1);
	~MatrixDisplay();
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

	byte num; // number of chained matrix displays
	byte *columns; // display content of all matrices
};
