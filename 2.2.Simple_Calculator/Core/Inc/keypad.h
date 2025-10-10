#ifndef KEYPAD_H
#define KEYPAD_H

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
  * @brief Resolve which key is being pressed otherwise return the error
  * @retval The key which was pressed
  */
int keypad(void);

#endif