#pragma once

class MatrixDisplay
{
public:
	MatrixDisplay(unsigned char num=1);
	~MatrixDisplay();
	void show(); // show current content
	void setPixel(unsigned char row, unsigned char column, unsigned char value);

private:
	const int data_pin  = 0; // data for Shift register 74HC595 on PORTB 0
	const int clock_pin = 1; // SHCP / clock pin for shift register on PORTB 1
	const int latch_pin = 2; // STCP / latch pin for shift register on PORTB 2

	unsigned char num; // number of chained matrix displays
	unsigned char *columns; // display content of all matrices
};
