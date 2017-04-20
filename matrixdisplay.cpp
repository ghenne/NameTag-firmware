#include "matrixdisplay.h"
#include "avr.h"

// This tables stores the bit patterns required to display all ASCII
// characters. Each letter is represented by up to 5 columns.
// First column specifies number of columns actually required.
#define FIRST_LETTER (' ' - 3)
#define LAST_LETTER '~'
const byte LETTERS[] = {
   4, 0b00100000, 0b01010101, 0b01010101, 0b01111000, 0b00000000, // ä = 29
   4, 0b00111000, 0b01000101, 0b01000101, 0b00111000, 0b00000000, // ö = 30
   4, 0b00111100, 0b01000001, 0b01000001, 0b01111100, 0b00000000, // ü = 31
   3, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // space = 32
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
   3, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00000000, // -
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
   2, 0b00101000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // :
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
const byte col_order[] = {4,5,1,6,2,0,7,3};
const byte row_order[] = {3,1,7,0,2,5,4,6};

// constructor: initialize data structures
MatrixDisplay::MatrixDisplay(byte num) : cols_(8*num)
{
	// allocate memory for (columnwise) display content
	columns_ = static_cast<byte*>(malloc(cols_));
	setFlipped(false);

	// declare selected pins of Port C as OUTPUT
	DDRC |= _BV(clock_pin) | _BV(latch_pin);
	// declare data pins on PortC as output
	DDRC |= _BV(2) |_BV(3) | _BV(4) | _BV(5);
	// set all eight pins of PortD as output
	DDRD = 0xFF;

	clear();
}

// destructor: free resources
MatrixDisplay::~MatrixDisplay()
{
	free(columns_);
}

// display content of columns_ by feeding the shift registers
void MatrixDisplay::show() const
{
	for(int row = 0; row < 8; ++row) {
		bitClear(PORTC, latch_pin); // clear latch
		for(byte *col = columns_, *end = columns_ + 8; col != end; ++col) {
			byte portc = PORTC;
			bitClear(portc, clock_pin);
			bitWrite(portc, 2, bitRead(*(col+24),row)); // set cols for first matrix
			bitWrite(portc, 3, bitRead(*(col+16),row)); // set cols for second matrix
			bitWrite(portc, 4, bitRead(*(col+8),row));  // set cols for third matrix
			bitWrite(portc, 5, bitRead(*(col+0),row));  // set cols for fourth
			PORTC = portc;
			bitSet(PORTC, clock_pin);
		}
		PORTD = 0xFF; // avoid glowing of prev/next row
		bitSet(PORTC, latch_pin); // set latch
		// select row to be displayed
		PORTD = ~_BV(row_order_[row]);
	}
}

// returns pointer to given column (considering col_order permutation)
byte* MatrixDisplay::columnPtr(byte column) const
{
	static const byte mask = 0b111; // bit mask for last 3 bits
	// reshuffle matrices?
	byte column_flipped = flipped_ ? cols_-1 - column : column;
	// reshuffle last 3 bits to match hardware wiring order
	column = (column_flipped & ~mask) | col_order[column_flipped & mask];
	return columns_ + column;
}

// clear content of columns start to end
void MatrixDisplay::clearColumns(int start, int end)
{
	if (start < 0) start = 0;
	if (end > cols_) end = cols_;

	for(; start < end; ++start)
		*columnPtr(start) = 0;
}

// set the column content of given column to given byte value
void MatrixDisplay::setColumn(int column, byte value)
{
	if (column >= 0 && column < cols_)
		*columnPtr(column) = value;
}

// set a single pixel
void MatrixDisplay::setPixel(byte row, byte column, byte value)
{
	if (row < 0 || row > 7 || column < 0 || column >= cols_)
		return;
	bitWrite(*columnPtr(column), row, value);
}

inline const byte* MatrixDisplay::letterStart(char ch) {
	if (ch < FIRST_LETTER || ch > LAST_LETTER) ch = '?';
	return LETTERS + 6*(ch - FIRST_LETTER);
}

// write a single char, starting at column
byte MatrixDisplay::setChar(char ch, int column)
{
	const byte *start = letterStart(ch);
	byte width = *start; ++start;
	const byte *end = start + width;
	for (; start != end; ++start, ++column)
		setColumn(column, *start);
	return width;
}

// write a string, starting at column
int MatrixDisplay::setString(const char *s, int column, char spacing) {
	while (*s != 0 && column < cols_) {
		column += setChar(*s, column);
		clearColumns(column, column+spacing);
		column += spacing;
		++s;
	}
	return column;
}

int MatrixDisplay::width(char ch)
{
	return *letterStart(ch);
}

// determine the width of the given string
int MatrixDisplay::width(const char *s, char spacing)
{
	int column = 0;
	while (*s != 0) {
		column += spacing + width(*s);
		++s;
	}
	return column;
}

// converts an integer to a string
char* MatrixDisplay::formatInt(char* digits, byte size, int value)
{
	if (size < 3) return digits;

	digits[--size] = '\0'; // terminating '\0'
	digits[--size] = '0'; // zero display if value == 0

	bool neg = false;
	if (value < 0) {
		neg = true;
		value = -value;
	} else if (value == 0)
		--size;

	for (; size != 0 && value != 0; --size) {
		digits[size] = '0' + (value % 10);
		value = value / 10;
	}

	if (neg) digits[size] = '-';
	else ++size;

	return digits + size;
}
void MatrixDisplay::setFlipped(bool flipped){
	flipped_ = flipped;
	if (flipped) {
		for (int i = 0; i < 8; ++i)
			row_order_[i] = ::row_order[7-i];
	} else {
		for (int i = 0; i < 8; ++i)
			row_order_[i] = ::row_order[i];
	}
	return;
}
