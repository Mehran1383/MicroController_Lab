#include "keypad.h"

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
	return (KEYPAD_PORT->IDR & 0xFF);
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
		
		if (c != NOT_DETECTED) {
			key = (r * 4) + c;
			keypad_write(0xF0);
			while (keypad_read_pin(c))
				;
			break;
		}
		
		keypad_write(0xF0);
	}
	return key;			
}
