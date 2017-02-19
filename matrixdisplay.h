#pragma once
#include "avr.h"

class MatrixDisplay
{
public:
	MatrixDisplay(byte num=1);
	~MatrixDisplay();
	inline byte num() const { return cols_ / 8; }
	inline byte cols() const { return cols_; }

	void show() const; // show current content

	inline void clear() { clearColumns(0, cols_); }
	void clearColumns(int start, int end);
	void setColumn(int column, byte value);

	void setPixel(byte row, byte column, byte value);
	// set matrix display to write character at column
	byte setChar(char ch, int column);
	// set matrix display to write string at column
	int setString(const char *s, int column=0, char spacing=1);
	// compute width (in columns) of text
	static int width(const char *s, char spacing=1);
	// write integer value into string digits of max size
	char *formatInt(char *digits, byte size, int value);

protected:
	byte *columnPtr(byte column) const;

private:
	const byte data_pin  = 0; // data for Shift register 74HC595 on PORTB 0
	const byte clock_pin = 1; // SHCP / clock pin for shift register on PORTB 1
	const byte latch_pin = 2; // STCP / latch pin for shift register on PORTB 2

	const byte cols_; // number of overall columns (8*num())
	byte *columns_; // display content of all matrices
};
