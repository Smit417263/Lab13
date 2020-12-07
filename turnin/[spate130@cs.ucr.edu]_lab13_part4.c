/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:023
 *	Assignment: Lab #13  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/5Egm1eP0AEQ
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

task tasks[3];
const unsigned short num_task = 3;
const unsigned long period_task = 1;
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

void Set_A2D_Pin(unsigned char pinNum) {
	ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
	static unsigned char i = 0;
	for ( i=0; i<15; i++ ) { asm("nop"); } 
}


unsigned short temp;
unsigned short temp_ud;

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
			Set_A2D_Pin(0x00);
            temp = ADC;
            Set_A2D_Pin(0x01);
            temp_ud = ADC;
			break;
		default:
			break;
	}
	return state;
}

unsigned char speed = 100;

enum Speed_SM{start_3};

int tick_3(int state_3){

switch(state_3){
    case start_3:
        break;
    default:
        state_3 = start_3;
        break;
}

switch(state_3){
case start_3:
    if (((temp < 500) && (temp >= 400)) || ((temp > 600) && (temp <= 700))){
        speed = 100;
    }
    else if(((temp < 400) && (temp >= 300)) || ((temp > 700) && (temp <= 800))){
        speed = 100;
    }
    else if(((temp < 300) && (temp >= 200)) || ((temp > 800) && (temp <= 900))){
        speed = 100;
    }
    else if((temp < 200) || (temp > 900)){
        speed = 100;
    }
    break;
default:
    break;
}
return state_3;
}




enum JoyLR_SM{start_2,wait,check};
int tick_2(int state_2) {				

const unsigned short left_reg = 500;
const unsigned short right_reg = 600;
static unsigned char p = 0x80;	
static unsigned char r = 0xFE;
static unsigned char temp2 = 0;

switch (state_2) {

case start_2:
    state_2 = wait;	
    p = 0x80;
    r = 0xFE;
    temp2 = 0;
    break;

case wait:
    if((temp >= left_reg) && (temp <= right_reg)){
        state_2 = wait;				
    }
    else{
        temp2 = 0;
        state_2 = check;
    }
    break;

case check:
    if((temp >= left_reg) && (temp <= right_reg)){
		state_2 = wait;
	}
	else{
		state_2 = check;
	}
	break;
    
default:			
    break;
}	
switch (state_2){

case check:
    if(temp2%speed == 0){
        if(temp > right_reg){
            if (p == 0x01) { 
                p = 0x01;		  			
            }
            else { 
                p = p >> 1;
            }
        }
        else if(temp < left_reg){
            if(p == 0x80){
                p = 0x80;
            }
            else{
                p = p << 1;
            }
        }
    }
    temp2 = temp2 + 1;
    break;

default:
    break;
}

PORTC = p;	
//PORTD = r;			
return state_2;
}



enum JoyUD_SM{start_4,wait_2,check_2};
int tick_4(int state_4) {				

const unsigned short left_reg = 500;
const unsigned short right_reg = 600;
static unsigned char p = 0x80;	
static unsigned char r = 0xFE;
static unsigned char temp2 = 0;

switch (state_4) {

case start_4:
    state_4 = wait_2;	
    p = 0x80;
    r = 0xFE;
    temp2 = 0;
    break;

case wait_2:
    if((temp_ud >= left_reg) && (temp_ud <= right_reg)){
        state_4 = wait_2;				
    }
    else{
        temp2 = 0;
        state_4 = check_2;
    }
    break;

case check_2:
    if((temp_ud >= left_reg) && (temp_ud <= right_reg)){
			state_4 = wait_2;
	}
	else{
		state_4 = check_2;
	}
	break;
    
default:			
    break;
}	
switch (state_4){

case check_2:
    if(temp2%speed == 0){
        if(temp_ud > right_reg){
            if (r == 0xFE) {
                r = 0xFE;
            }
            else{
                r = (r >> 1) |0x80;
            }
        }
        else if(temp_ud < left_reg){
            if(r == 0xEF){
                r = 0xEF;
            }
            else{
                r = (r << 1) | 0x01;
            }
        }
    }
    temp2 = temp2 + 1;
    break;

default:
    break;
}

//PORTC = p;	
PORTD = r;			
return state_4;
}





int main(void) {

	DDRA=0x00; PORTA=0xFF;
	DDRC=0xFF; PORTB=0x00;
	DDRD=0xFF; PORTC=0x00;

	A2D_init();

	unsigned char i = 0;
	tasks[i].state = start_1;
	tasks[i].period = 1;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &tick_1;
	i++;
    	tasks[i].state = start_4;
    	tasks[i].period = 1;
    	tasks[i].elapsedTime = 0;
    	tasks[i].TickFct = &tick_4;
	i++;
	tasks[i].state = start_2;
    	tasks[i].period = 1;
    	tasks[i].elapsedTime = 0;
    	tasks[i].TickFct = &tick_2;
	TimerSet(1);
	TimerOn();

	while (1) {}
	return 1;
}
