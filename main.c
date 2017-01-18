#include <avr/io.h>
#include <util/delay.h>

void setup(){
    DDRD=0b00000010;
    PORTD = 0b00000001;
}

int output(int bit, int input,int state){  // use it like this : PORTD = output(x, PIND, statex);
    if (state == 1){                 // if the output state is 1
        return input | (1 << bit);   // return the input witch a mask of one bit with 1 on the place
                                     // of the bit we would switch.
    }                                //    input  =   01001000
                                     //    mask   = | 00000001   // we would switch the first bit to 1
                                     //    -------------------
                                     //    output =   01001001

    else if (state == 0){            // if the output state ist 0 it is an an tricky
        return input & ~(1 << bit);  // return the input value combined with inverted two Pots of bit
    }
    else{                            // if the state is not 1 and not 0
        return input;                // return the input value
    }
}

int input (int bit, int input){
    if(input & (1 << bit)) {
        return 1;
    }
    else{
        return 0;
    }
}

int button(){
    if (PIND & 0b00000001) {
        PORTD = PIND | 0b00000010;
    }
    else
    {
        PORTD = PIND & ~ 0b00000010;
    }
    return 0;
}

int button2(){
    if (input(0, PIND)) {
        PORTD = output(1,PIND,1);
    }
    else
    {
        PORTD = output(1,PIND,0);
    }
    return 0;
}





int main(void)
{
    setup();
    while(1) {
        button2();

    }
}
    /*
    DDRD = 0b11111110; // pin 1 an Port A als Output
    unsigned char state;
    while(1) {
       state = PORTD; // PORTB status einlesen
       state = state ^ 01010101; // Bit switchen
       PORTD = state;
       //PORTD = 0b00000001; // PORTB schreiben
       _delay_ms(100);
    }
    return 0;
    */
