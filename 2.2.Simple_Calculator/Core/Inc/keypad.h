#include "stm32f4xx_hal.h"

/*
	GPIOE0-3 -> C1-4 as INPUT
	GPIOE4-7 -> R1-4 as OUTPUT
*/
#define KEYPAD_PORT GPIOE

// Error types
#define NOT_DETECTED -1
#define INVALID_INPUT -2
#define ERROR -3

/**
  * @brief Wrie data on keypad
	* @param Data to be written
  * @retval None
  */
void keypad_write(const char data);

/**
  * @brief Read state of a pin from keypad port
	* @param Pin to be read
  * @retval State of the pin
  */
int keypad_read_pin(const char pin);

/**
  * @brief Read keypad port
  * @retval Value of keypad port
  */
int keypad_read(void);

/**
  * @brief Resolve which key is being pressed
  * @retval The key which is pressed
  */
int keypad(void);
