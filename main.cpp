#include "avr.h"
#include <util/delay.h>
#include <stdint.h>
#include "matrixdisplay.h"

int main(void)
{
	MatrixDisplay m(1);
#if 1
	m.setChar('!', 1, 0);
	m.setChar('A', 3, 0);
#else
	for (byte i = 0; i < 8; ++i) {
		m.setPixel(i,i, 1);
		m.setPixel(i,7-i, 1);
	}
#endif

	while(1) {
		m.show();
	}
}

