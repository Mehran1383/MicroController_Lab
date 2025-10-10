#include "stm32f4xx.h"
#include "keypad.h"

/*
	GPIOE0-3 -> C1-4 as INPUT
	GPIOE4-7 -> R1-4 as OUTPUT
*/
#define KEYPAD_PORT GPIOE

void keypad_write(const char data)
{
	KEYPAD_PORT->ODR = (KEYPAD_PORT->ODR & 0xFFFFFF00) | (data & 0xFF);
}

int keypad_read_pin(const char pin)
{
	return (KEYPAD_PORT->IDR >> pin) & 0x1;
}

int keypad_read(void)
{
	return KEYPAD_PORT->IDR & 0xFF;
}

int keypad(void)
{
	char row[] = {0x10, 0x20, 0x40, 0x80};
	int key = NOT_DETECTED, c = NOT_DETECTED;
	
	for (int r = 0; r < 4; r++) {
		keypad_write(row[r]);
		
		if (keypad_read_pin(0)) 
			c = 0;
		else if (keypad_read_pin(1)) 
			c = 1;
		else if (keypad_read_pin(2)) 
			c = 2;
		else if (keypad_read_pin(3)) 
			c = 3;
		
		// Reset pins to initial state
		keypad_write(0xF0);
		if (c != NOT_DETECTED) {
			key = (r * 4) + c;
			while (keypad_read_pin(c))
				;
			break;
		}		
	}
	return key;			
}
