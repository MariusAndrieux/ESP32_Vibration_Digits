#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"

// GPIO definitions
#define BUTTON_PIN GPIO_NUM_32
#define LED_PIN    GPIO_NUM_2

const gpio_num_t segments_pins[8] = {
    GPIO_NUM_33,   // A
    GPIO_NUM_25,   // B
    GPIO_NUM_26,   // C
    GPIO_NUM_27,   // D
    GPIO_NUM_14,   // E
    GPIO_NUM_12,   // F
    GPIO_NUM_19,   // G
    GPIO_NUM_15    // DP
};

const gpio_num_t digits_pins[4] = {
    GPIO_NUM_16,   // D1
    GPIO_NUM_17,   // D2
    GPIO_NUM_5,    // D3
    GPIO_NUM_18    // D4
};

const uint8_t digits_segments[10] = {
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

static const char *Button_TAG = "BUTTON_TASK";
static const char *Display_TAG = "DISPLAY_TASK";

volatile int press_count = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void set_segments(uint8_t value) {
    for (int i = 0; i < 8; i++) {
        int level = (value >> i) & 0x01;
        gpio_set_level(segments_pins[i], level);
    }
}

void clear_digits(void) {
    for (int i = 0; i < 4; i++) {
        gpio_set_level(digits_pins[i], 1);
    }
}

void display_task(void *param) {
    while (1) {
        int num;
        portENTER_CRITICAL(&mux);
        num = 42;// press_count % 10000;  // Limit to 4 digits
        portEXIT_CRITICAL(&mux);

        int digits[4] = {
            (num / 1000) % 10,
            (num / 100) % 10,
            (num / 10) % 10,
            num % 10
        };

        for (int i = 0; i < 4; i++) {
            clear_digits();  // Turn off all digits
            set_segments(digits_segments[digits[i]] & 0x7F); // Clear DP
            gpio_set_level(digits_pins[i], 0);  // Enable digit
            vTaskDelay(pdMS_TO_TICKS(2));
        }
    }
}

void button_task(void *param) {
    int last_state = 1;

    while (1) {
        int state = gpio_get_level(BUTTON_PIN);
        if (state == 0 && last_state == 1) {
            vTaskDelay(pdMS_TO_TICKS(10)); // debounce delay
            if (gpio_get_level(BUTTON_PIN) == 0) {
                portENTER_CRITICAL(&mux);
                press_count++;
                portEXIT_CRITICAL(&mux);
                ESP_LOGI(Button_TAG, "Button pressed. Count: %d", press_count);
            }
        }
        gpio_set_level(LED_PIN, state);  // LED reflects current button state
        last_state = state;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void app_main(void) {
    // Configure LED GPIO
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Configure Button GPIO
    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    // Configure Segment GPIOs
    for (int i = 0; i < 8; i++) {
        gpio_reset_pin(segments_pins[i]);
        gpio_set_direction(segments_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(segments_pins[i], 1);
    }

    // Configure Digit GPIOs
    for (int i = 0; i < 4; i++) {
        gpio_reset_pin(digits_pins[i]);
        gpio_set_direction(digits_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(digits_pins[i], 1); // disable all digits initially
    }

    xTaskCreate(display_task, "display_task", 2048, NULL, 2, NULL);
    xTaskCreate(button_task, "button_task", 2048, NULL, 2, NULL);
}
