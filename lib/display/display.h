#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "driver/gpio.h"

// Nombre de digits
#define DISPLAY_DIGITS 4

// Display  
#define SEGMENT_ON  0x0
#define SEGMENT_OFF 0x1
#define DIGIT_ON    0x1
#define DIGIT_OFF   0x0

// Delay
#define DISPLAY_INTERVAL 5

void display_init(void);
void display_set_value(int value, int intervall);
void set_segments(uint8_t value);
void clear_digits(void);

#endif