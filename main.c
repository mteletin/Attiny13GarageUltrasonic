/*
 * echoLightBarProj.c
 *
 * Created: 10/8/2017 10:44:49 AM
 * Author : mirce
 */ 

#define F_CPU 9600000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned int volatile R = 0;
unsigned int volatile G = 0;
unsigned int volatile B = 0;
unsigned int volatile RR = 0;
unsigned int volatile RG = 0;
unsigned int volatile RB = 0;
unsigned int volatile x = 0;
volatile uint32_t ticks = 0;
volatile unsigned char delta = 0;

int main(void)
{
	cli();
	DDRB = 0b001111, PORTB = 0b000000;
	TCCR0B |= (1<<CS01);
	TIMSK0 |= (1<<TOIE0);
	GIMSK |= (1 << PCIE);
	PCMSK |= (1 << PCINT4);
	sei();
	delta = 0;
	while(1)
	{
		PORTB &= 0b110111, _delay_us(5);
		PORTB |= 0b001000, _delay_us(10);
		PORTB &= 0b110111;
		R = (1<<(32-delta))-1;
		G = (1<<(16-abs(delta-16)))-1;
		B = (1<<delta)-1;
		_delay_ms(50);
	}
}

ISR(TIM0_OVF_vect)
{
	if (x == 0) x = 32, RR = R, RG = G, RB = B;
	PORTB = (RB & 1)+((RR & 1)*2)+((RG & 1)*4);
	RR = RR >> 1, RG = RG >> 1, RB = RB >> 1;
	x--, ticks++;
}

ISR(PCINT0_vect)
{
	if (PINB >> 4 & 1) ticks = 0;
	else if (ticks > 63) delta = 31; else delta = (ticks >> 2) & 0b11111;
}