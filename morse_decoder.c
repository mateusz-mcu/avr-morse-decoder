/*
 * Start.c
 *
 * Created: 24/02/2026 7:29:20 PM
 * Author : mateusz
 */ 

#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 3333333UL

#define DOT 1
#define DASH 4
#define GAP 10

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>


volatile uint32_t counter = 0;


typedef struct 
{
    uint8_t pattern[5];
    uint8_t length;     
    char letter;
} MorseCode;


MorseCode alphabet[] = {
    {{0, 1}, 2, 'A'},
    {{1, 0, 0, 0}, 4, 'B'},
	{{1, 0, 1, 0}, 4, 'C'},
	{{1, 0, 0}, 3, 'D'},
	{{0}, 1, 'E'},
	{{0, 0, 1, 0}, 4, 'F'},
	{{1, 1, 0}, 3, 'G'},
	{{0, 0, 0, 0}, 4, 'H'},
	{{0,0}, 2, 'I'},
	{{0, 1, 1, 1}, 4, 'J'},
	{{1, 0, 1}, 3, 'K'},
	{{0, 1, 0, 0}, 4, 'L'},
	{{1, 1}, 2, 'M'},
	{{1, 0}, 2, 'N'},
	{{1, 1, 1}, 3, 'O'},
	{{0, 1, 1, 0}, 4, 'P'},
	{{1, 1, 0, 1}, 4, 'Q'},
	{{0, 1, 0}, 3, 'R'},
	{{0, 0, 0}, 3, 'S'},
	{{1}, 1, 'T'},
	{{0, 0, 1}, 3, 'U'},		
	{{0, 0, 0, 1}, 4, 'V'},	
	{{0, 1, 1}, 3, 'W'},	
	{{1, 0, 0, 1}, 4, 'X'},		
	{{1, 0, 1, 1}, 4, 'Y'},	
	{{1, 1, 0, 0}, 4, 'Z'}	
				
};


void init_timer()
{
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; 
    TCA0.SINGLE.PER = 1389;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
}

ISR(TCA0_OVF_vect)
{
	
		counter++;
		TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
				
}

void init_uart(void)
{
	PORTB.DIRSET = PIN0_bm;
	USART3.BAUD = 1389;
	USART3.CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc;
	USART3.CTRLB = USART_TXEN_bm;
}

void uart_send_char(char c) 
{
	while (!(USART3.STATUS & USART_DREIF_bm))
	{

	}
	
	USART3.TXDATAL = c;
}

void flash_dot(void)
{
	PORTF.OUTCLR = PIN5_bm;
	_delay_ms(100);
	PORTF.OUTSET = PIN5_bm;
}

void flash_dash(void)
{
	PORTF.OUTCLR = PIN5_bm;
	_delay_ms(300);
	PORTF.OUTSET = PIN5_bm;
}

void flash(void) 
{
	for (int i = 0; i < 3; i++)
	{
		PORTF.OUTCLR = PIN5_bm;
		_delay_ms(250);
		PORTF.OUTSET = PIN5_bm;
		_delay_ms(250);
	}
}



	
int main(void)
{
	
	uint8_t code[10] = {};
	uint32_t start = 0;
	uint32_t duration = 0;
	uint32_t silence = 0;
	uint8_t prev_button = 0;
	uint8_t i = 0;
	uint8_t space = 0;
	
	
	int alphabet_size = sizeof(alphabet) / sizeof(alphabet[0]);
	
	PORTF.DIRSET = PIN5_bm;
	PORTF.DIRCLR = PIN6_bm;
	PORTF.OUTSET = PIN5_bm;
	
	sei();
	init_timer();
	init_uart();
	
	
	i = 0;
	space = 0;
	silence = counter;
	prev_button = 0;
	
	flash();
	

	
while(1)
{
	
	uint8_t button = !(PORTF.IN & PIN6_bm);
	
	if (button == 1 && prev_button == 0) 
	{
		start = counter;
	}
	
	if (button == 0 && prev_button == 1)
	{
		duration = counter - start;
		silence = counter;
			
		if (duration >= DASH) 
		{
			flash_dash();
			code[i] = 1;
		}
			
		else 
		{
			flash_dot();
			code[i] = 0;
		}
		i++;
	}
	
	if (button == 0 && counter - silence >= GAP && i > 0)
	{
		
		for (int a = 0; a < alphabet_size; a++)
		{
			if (i == alphabet[a].length)
			{
				uint8_t match = 1;
				for (int j = 0; j < i; j++) 
				{
					if (code[j] != alphabet[a].pattern[j])
					{
						match = 0; 
						break;  
					}
				}

				if (match == 1)
				{
					space = 1;
					uart_send_char(alphabet[a].letter);
					break;
				}
				
			}
		}
		i = 0;
		silence = counter;
		
	}
	
	if (button == 0 && i == 0 && space == 1 &&(counter - silence) >= (GAP * 3)) 
	{
		uart_send_char(' ');
		space = 0; 
		silence = counter;
	}
	
	prev_button = button;
}
}

