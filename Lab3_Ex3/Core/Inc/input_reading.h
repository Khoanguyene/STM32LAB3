/*
 * input_reading.h
 *
 *  Created on: Oct 27, 2026
 *      Author: khoan
 */

#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_
#include <stdint.h>

#define BUTTON_MODE_INDEX 0U
#define BUTTON_INC_INDEX  1U
#define BUTTON_SET_INDEX  2U

void button_reading_10ms(void);
uint8_t button_is_pressed(uint8_t index);


#endif /* INC_INPUT_READING_H_ */
