#include "matrixdisplay.h"
#include "avr.h"
#include <util/delay.h>

// PORT D serves as row selector
// PORT B (3 pins) is used to feed the shift registers

// row / column re-ordering
const int col_order[] = {7,0,1,2,3,4,5,6};
const int row_order[] = {4,5,6,7,0,1,2,3};

// constructor: initialize data structures
MatrixDisplay::MatrixDisplay(unsigned char num) : num(num)
{
	columns = static_cast<unsigned char*>(malloc(8*num));
	for(unsigned char *col = columns, *end = columns + 8*num; col < end; ++col)
		*col = 0;

	// declare selected pins of PortB as output
	DDRB |= (1 << data_pin) | (1 << clock_pin) | (1 << latch_pin);
	// set all eight pin of PortD as output
	DDRD = 0xFF;
}

MatrixDisplay::~MatrixDisplay()
{
	free(columns);
}

void MatrixDisplay::show()
{
	for(int row = 0; row < 8; ++row) {
		bitClear(PORTB, latch_pin); // clear latch
		for(unsigned char *col = columns, *end = columns + 8*num; col < end; ++col) {
			bitClear(PORTB, clock_pin);
			bitWrite(PORTB, data_pin, bitRead(*col, row));
			bitSet(PORTB, clock_pin);
		}
		PORTD = 0xFF; // avoid glowing of prev/next row
		bitSet(PORTB, latch_pin); // set latch
		// select row to be displayed
		PORTD = ~(1 << row_order[row]);
	}
}

void MatrixDisplay::setPixel(unsigned char row, unsigned char column, unsigned char value)
{
	unsigned char display = column / 8;
	column = col_order[column % 8];
	unsigned char *col = columns + 8*display + column;
	bitWrite(*col, row, value);
}
