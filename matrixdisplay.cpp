#include "matrixdisplay.h"
#include "avr.h"

// This tables stores the bit patterns required to display all ASCII
// characters. Each letter is represented by up to 5 columns.
// First column specifies number of columns actually required.
const byte LETTERS[] = {
   3, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // space
   1, 0b01011111, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // !
   3, 0b00000011, 0b00000000, 0b00000011, 0b00000000, 0b00000000, // "
   5, 0b00010100, 0b00111110, 0b00010100, 0b00111110, 0b00010100, // #
   4, 0b00100100, 0b01101010, 0b00101011, 0b00010010, 0b00000000, // $
   5, 0b01100011, 0b00010011, 0b00001000, 0b01100100, 0b01100011, // %
   5, 0b00110110, 0b01001001, 0b01010110, 0b00100000, 0b01010000, // &
   1, 0b00000011, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // '
   3, 0b00011100, 0b00100010, 0b01000001, 0b00000000, 0b00000000, // (
   3, 0b01000001, 0b00100010, 0b00011100, 0b00000000, 0b00000000, // )
   5, 0b00101000, 0b00011000, 0b00001110, 0b00011000, 0b00101000, // *
   5, 0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000, // +
   2, 0b10110000, 0b01110000, 0b00000000, 0b00000000, 0b00000000, // ,
   4, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00000000, // -
   2, 0b01100000, 0b01100000, 0b00000000, 0b00000000, 0b00000000, // .
   4, 0b01100000, 0b00011000, 0b00000110, 0b00000001, 0b00000000, // /
   4, 0b00111110, 0b01000001, 0b01000001, 0b00111110, 0b00000000, // 0
   3, 0b01000010, 0b01111111, 0b01000000, 0b00000000, 0b00000000, // 1
   4, 0b01100010, 0b01010001, 0b01001001, 0b01000110, 0b00000000, // 2
   4, 0b00100010, 0b01000001, 0b01001001, 0b00110110, 0b00000000, // 3
   4, 0b00011000, 0b00010100, 0b00010010, 0b01111111, 0b00000000, // 4
   4, 0b00100111, 0b01000101, 0b01000101, 0b00111001, 0b00000000, // 5
   4, 0b00111110, 0b01001001, 0b01001001, 0b00110000, 0b00000000, // 6
   4, 0b01100001, 0b00010001, 0b00001001, 0b00000111, 0b00000000, // 7
   4, 0b00110110, 0b01001001, 0b01001001, 0b00110110, 0b00000000, // 8
   4, 0b00000110, 0b01001001, 0b01001001, 0b00111110, 0b00000000, // 9
   2, 0b01010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // :
   2, 0b10000000, 0b01010000, 0b00000000, 0b00000000, 0b00000000, // ;
   3, 0b00010000, 0b00101000, 0b01000100, 0b00000000, 0b00000000, // <
   3, 0b00010100, 0b00010100, 0b00010100, 0b00000000, 0b00000000, // =
   3, 0b01000100, 0b00101000, 0b00010000, 0b00000000, 0b00000000, // >
   4, 0b00000010, 0b01011001, 0b00001001, 0b00000110, 0b00000000, // ?
   5, 0b00111110, 0b01001001, 0b01010101, 0b01011101, 0b00001110, // @
   4, 0b01111110, 0b00010001, 0b00010001, 0b01111110, 0b00000000, // A
   4, 0b01111111, 0b01001001, 0b01001001, 0b00110110, 0b00000000, // B
   4, 0b00111110, 0b01000001, 0b01000001, 0b00100010, 0b00000000, // C
   4, 0b01111111, 0b01000001, 0b01000001, 0b00111110, 0b00000000, // D
   4, 0b01111111, 0b01001001, 0b01001001, 0b01000001, 0b00000000, // E
   4, 0b01111111, 0b00001001, 0b00001001, 0b00000001, 0b00000000, // F
   4, 0b00111110, 0b01000001, 0b01001001, 0b01111010, 0b00000000, // G
   4, 0b01111111, 0b00001000, 0b00001000, 0b01111111, 0b00000000, // H
   3, 0b01000001, 0b01111111, 0b01000001, 0b00000000, 0b00000000, // I
   4, 0b00110000, 0b01000000, 0b01000001, 0b00111111, 0b00000000, // J
   4, 0b01111111, 0b00001000, 0b00010100, 0b01100011, 0b00000000, // K
   4, 0b01111111, 0b01000000, 0b01000000, 0b01000000, 0b00000000, // L
   5, 0b01111111, 0b00000010, 0b00001100, 0b00000010, 0b01111111, // M
   5, 0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111, // N
   4, 0b00111110, 0b01000001, 0b01000001, 0b00111110, 0b00000000, // O
   4, 0b01111111, 0b00001001, 0b00001001, 0b00000110, 0b00000000, // P
   4, 0b00111110, 0b01000001, 0b01000001, 0b10111110, 0b00000000, // Q
   4, 0b01111111, 0b00001001, 0b00001001, 0b01110110, 0b00000000, // R
   4, 0b01000110, 0b01001001, 0b01001001, 0b00110010, 0b00000000, // S
   5, 0b00000001, 0b00000001, 0b01111111, 0b00000001, 0b00000001, // T
   4, 0b00111111, 0b01000000, 0b01000000, 0b00111111, 0b00000000, // U
   5, 0b00001111, 0b00110000, 0b01000000, 0b00110000, 0b00001111, // V
   5, 0b00111111, 0b01000000, 0b00111000, 0b01000000, 0b00111111, // W
   5, 0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011, // X
   5, 0b00000111, 0b00001000, 0b01110000, 0b00001000, 0b00000111, // Y
   4, 0b01100001, 0b01010001, 0b01001001, 0b01000111, 0b00000000, // Z
   2, 0b01111111, 0b01000001, 0b00000000, 0b00000000, 0b00000000, // [
   4, 0b00000001, 0b00000110, 0b00011000, 0b01100000, 0b00000000, // \ backslash
   2, 0b01000001, 0b01111111, 0b00000000, 0b00000000, 0b00000000, // ]
   3, 0b00000010, 0b00000001, 0b00000010, 0b00000000, 0b00000000, // hat
   4, 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b00000000, // _
   2, 0b00000001, 0b00000010, 0b00000000, 0b00000000, 0b00000000, // `
   4, 0b00100000, 0b01010100, 0b01010100, 0b01111000, 0b00000000, // a
   4, 0b01111111, 0b01000100, 0b01000100, 0b00111000, 0b00000000, // b
   4, 0b00111000, 0b01000100, 0b01000100, 0b00101000, 0b00000000, // c
   4, 0b00111000, 0b01000100, 0b01000100, 0b01111111, 0b00000000, // d
   4, 0b00111000, 0b01010100, 0b01010100, 0b00011000, 0b00000000, // e
   3, 0b00000100, 0b01111110, 0b00000101, 0b00000000, 0b00000000, // f
   4, 0b10011000, 0b10100100, 0b10100100, 0b01111000, 0b00000000, // g
   4, 0b01111111, 0b00000100, 0b00000100, 0b01111000, 0b00000000, // h
   3, 0b01000100, 0b01111101, 0b01000000, 0b00000000, 0b00000000, // i
   4, 0b01000000, 0b10000000, 0b10000100, 0b01111101, 0b00000000, // j
   4, 0b01111111, 0b00010000, 0b00101000, 0b01000100, 0b00000000, // k
   3, 0b01000001, 0b01111111, 0b01000000, 0b00000000, 0b00000000, // l
   5, 0b01111100, 0b00000100, 0b01111100, 0b00000100, 0b01111000, // m
   4, 0b01111100, 0b00000100, 0b00000100, 0b01111000, 0b00000000, // n
   4, 0b00111000, 0b01000100, 0b01000100, 0b00111000, 0b00000000, // o
   4, 0b11111100, 0b00100100, 0b00100100, 0b00011000, 0b00000000, // p
   4, 0b00011000, 0b00100100, 0b00100100, 0b11111100, 0b00000000, // q
   4, 0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b00000000, // r
   4, 0b01001000, 0b01010100, 0b01010100, 0b00100100, 0b00000000, // s
   3, 0b00000100, 0b00111111, 0b01000100, 0b00000000, 0b00000000, // t
   4, 0b00111100, 0b01000000, 0b01000000, 0b01111100, 0b00000000, // u
   5, 0b00011100, 0b00100000, 0b01000000, 0b00100000, 0b00011100, // v
   5, 0b00111100, 0b01000000, 0b00111100, 0b01000000, 0b00111100, // w
   5, 0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100, // x
   4, 0b10011100, 0b10100000, 0b10100000, 0b01111100, 0b00000000, // y
   3, 0b01100100, 0b01010100, 0b01001100, 0b00000000, 0b00000000, // z
   3, 0b00001000, 0b00110110, 0b01000001, 0b00000000, 0b00000000, // {
   1, 0b01111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // |
   3, 0b01000001, 0b00110110, 0b00001000, 0b00000000, 0b00000000, // }
   4, 0b00001000, 0b00000100, 0b00001000, 0b00000100, 0b00000000, // ~
};

// PORT D serves as row selector
// PORT B (3 pins) is used to feed the shift registers

// row / column re-ordering
const byte col_order[] = {7,6,5,4,3,2,1,0};
const byte row_order[] = {4,5,6,7,0,1,2,3};

// constructor: initialize data structures
MatrixDisplay::MatrixDisplay(byte num) : cols_(8*num)
{
	columns_ = static_cast<byte*>(malloc(cols_));

	// declare selected pins of PortB as output
	DDRB |= (1 << data_pin) | (1 << clock_pin) | (1 << latch_pin);
	// set all eight pin of PortD as output
	DDRD = 0xFF;

	clear();
}

MatrixDisplay::~MatrixDisplay()
{
	free(columns_);
}

void MatrixDisplay::show() const
{
	for(int row = 0; row < 8; ++row) {
		bitClear(PORTB, latch_pin); // clear latch
		for(byte *col = columns_, *end = columns_ + cols_; col != end; ++col) {
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

byte* MatrixDisplay::columnPtr(byte column) const
{
	static const byte mask = 0b111; // bit mask for last 3 bits
	// reshuffle last 3 bits to match hardware wiring order
	column = (column & ~mask) | col_order[column & mask];
	return columns_ + column;
}

void MatrixDisplay::clearColumns(char start, char end)
{
	if (start < 0) start = 0;
	if (end > cols_) end = cols_;

	for(; start < end; ++start)
		*columnPtr(start) = 0;
}

void MatrixDisplay::setColumn(char column, byte value)
{
	if (column >= 0 && column < cols_)
		*columnPtr(column) = value;
}

void MatrixDisplay::setPixel(byte row, byte column, byte value)
{
	if (row < 0 || row > 7 || column < 0 || column >= cols_)
		return;
	bitWrite(*columnPtr(column), row, value);
}

byte MatrixDisplay::setChar(unsigned char ch, byte column)
{
	const byte *start = LETTERS + 6*(ch - 32);
	byte width = *start; ++start;
	const byte *end = start + width;
	for (; start != end; ++start, ++column)
		setColumn(column, *start);
	return width;
}

char MatrixDisplay::setString(const char *s, char column, char spacing) {
	while (*s != 0) {
		column += setChar(*s, column);
		clearColumns(column, column+spacing);
		column += spacing;
		++s;
	}
	return column;
}

byte MatrixDisplay::width(const char *s, char spacing)
{
	byte column = 0;
	while (*s != 0) {
		column += spacing + LETTERS[6 * (*s - 32)];
		++s;
	}
	return column;
}

const char* MatrixDisplay::formatInt(int value)
{
	const byte LEN = 10; // number of digits
	static char digits[LEN];
	byte digit = LEN-1;
	digits[digit--] = '\0'; // terminating '\0'
	digits[digit] = '0'; // zero display if value == 0

	bool neg = (value < 0);
	value = abs(value);

	for (; digit != 0 && value != 0; --digit) {
		digits[digit] = '0' + (value % 10);
		value = value / 10;
	}

	if (neg) digits[digit] = '-';
	return digits + digit;
}
