/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:023
 *	Assignment: Lab #13  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link:
 */


#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void A2D_init() {
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}
      
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */

	unsigned short temp;
	A2D_init();

    while (1) {
		temp = ADC;
		PORTB = input & 0xFF;
		PORTD = input >> 8;
    }
    return 1;
}
