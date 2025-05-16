#include "display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"


static const gpio_num_t segments_pins[8] = {
    GPIO_NUM_33,   // A
    GPIO_NUM_25,   // B
    GPIO_NUM_26,   // C
    GPIO_NUM_27,   // D
    GPIO_NUM_14,   // E
    GPIO_NUM_12,   // F
    GPIO_NUM_19,   // G
    GPIO_NUM_15    // DP
};

static const gpio_num_t digits_pins[4] = {
    GPIO_NUM_16,   // D1
    GPIO_NUM_17,   // D2
    GPIO_NUM_5,    // D3
    GPIO_NUM_18    // D4
};

static const uint8_t digits_segments[10] = {
    0b00111111, // 0 = A B C D E F
    0b00000110, // 1 = B C
    0b01011011, // 2 = A B D E G
    0b01001111, // 3 = A B C D G
    0b01100110, // 4 = B C F G
    0b01101101, // 5 = A C D F G
    0b01111101, // 6 = A C D E F G
    0b00000111, // 7 = A B C
    0b01111111, // 8 = A B C D E F G
    0b01101111  // 9 = A B C D F G
};

void display_init(void) {
    // Configure Segment GPIOs
    for (int i = 0; i < 8; i++) {
        gpio_reset_pin(segments_pins[i]);
        gpio_set_direction(segments_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(segments_pins[i], SEGMENT_OFF);
    }

    // Configure Digit GPIOs
    for (int i = 0; i < 4; i++) {
        gpio_reset_pin(digits_pins[i]);
        gpio_set_direction(digits_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(digits_pins[i], DIGIT_OFF); // disable all digits initially
    }
}

void set_segments(uint8_t value) {
    for (int i = 0; i < 8; i++) {
        int level = (value >> i) & 0x01;
        gpio_set_level(segments_pins[i], !level);
    }
}

void clear_digits(void) {
    for (int i = 0; i < 4; i++) {
        gpio_set_level(digits_pins[i], 0);
    }
}

void display_set_value(int value, int intervall) {
    
    value %= 10000;

    int digits[4] = {
        (value / 1000) % 10,
        (value / 100) % 10,
        (value / 10) % 10,
        value % 10
    };

    for (int i = 0; i < 4; i++) {
        clear_digits();  // Turn off all digits
        set_segments(digits_segments[digits[i]] & 0x7F); // Clear DP
        gpio_set_level(digits_pins[i], DIGIT_ON);  // Enable digit
        vTaskDelay(pdMS_TO_TICKS(intervall));
    }
}