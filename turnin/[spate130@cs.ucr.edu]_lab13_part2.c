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
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef struct task{
        int state;
        unsigned long period;
        unsigned long elapsedTime;
        int(*TickFct)(int);
}task;

task tasks[2];
const unsigned short num_task = 2;
const unsigned long period_task = 50;
volatile unsigned char TimerFlag = 0; 
unsigned long _avr_timer_M = 1; 
unsigned long _avr_timer_cntcurr = 0; 
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void TimerOn() {
	TCCR1B 	= 0x0B;	
	OCR1A 	= 125;	
	TIMSK1 	= 0x02; 
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;	
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B 	= 0x00; 
}

void TimerISR() {
	unsigned char i;
	for(i = 0; i < num_task; ++i){
		if(tasks[i].elapsedTime>=tasks[i].period){
			tasks[i].state=tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime=0;
		}
		tasks[i].elapsedTime += period_task;
	}
}
ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) { 	
		TimerISR(); 				
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void A2D_init(){
	ADCSRA |=(1<<ADEN)|(1<<ADSC)|(1<<ADATE);
}


unsigned short temp;


enum ADC_SM{start_1};

int tick_1(int state){
	switch(state){
		case start_1:
			break;
		default:
			state = start_1;
			break;
	}
	switch(state){
		case start_1:
			temp = ADC;
			break;
		default:
			break;
	}
	return state;
}



enum Joy_SM{start_2,wait,right,left};

int tick_2(int state_2) {				
static unsigned short left_reg = 400;
static unsigned short right_reg = 550;
static unsigned char p = 0x80;	
static unsigned char r = 0xFE;  

switch (state_2) {
case start_2:
    state_2 = wait;	
    p = 0x80;
    r = 0xFE;
    break;

case wait:
    if((temp >= left_reg) && (temp <= right_reg)){
        state_2 = wait;				
    }
    else if(temp < left_reg){
        state_2 = left;
    }
    else if (temp > right_reg){
        state_2 = right;
    }
    break;

case right:
    if(temp > right_reg){
        state_2 = right;
    }
    else{
        state_2 = wait;
    }
    break;

case left:
    if(temp < left_reg){
        state_2 = left;
    }
    else{
        state_2 = wait;
    }
    break;


default:			
    break;
}	
switch (state_2) {
case right:	
    if (p == 0x01) { 
        p = 0x80;		  			
    }
    else { 
        p = p >> 1;
    }
    break;
case left:
    if(p == 0x80){
        p = 0x01;
    }
    else{
        p = p << 1;
    }
default:
    break;
}

PORTC = p;	
PORTD = r;			
return state_2;
}




int main(void) {

	DDRA=0x00;PORTA=0xFF;
	DDRC=0xFF;PORTB=0x00;
	DDRD=0xFF;PORTC=0x00;

	A2D_init();

	unsigned char i=0;
	tasks[i].state = start_1;
	tasks[i].period=100;
	tasks[i].elapsedTime=0;
	tasks[i].TickFct=&tick_1;
	i++;
	tasks[i].state=start_2;
    	tasks[i].period=100;
    	tasks[i].elapsedTime=0;
	tasks[i].TickFct=&tick_2;
	TimerSet(100);
	TimerOn();

	while (1) {}
	return 1;
}
