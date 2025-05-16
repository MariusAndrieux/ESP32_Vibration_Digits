#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "display.h"

// GPIO definitions
#define BUTTON_PIN      GPIO_NUM_32
#define LED_PIN         GPIO_NUM_2
#define POTENTIO_CHAN   ADC1_CHANNEL_6

static const char *Button_TAG = "BUTTON_TASK";
static const char *Display_TAG = "DISPLAY_TASK";
static const char *Poten_TAG = "POTENTIO_TASK";
static const char *main_func = "MAIN";

volatile int press_count = 0;

void display_task(void *param) {
    while (1) {

        display_set_value(press_count, 95*adc1_get_raw(POTENTIO_CHAN)/0xFFF  + 5);
    }
}

void button_task(void *param) {
    int last_state = 1;

    while (1) {
        int state = gpio_get_level(BUTTON_PIN);
        if (state == 0 && last_state == 1) {
            vTaskDelay(pdMS_TO_TICKS(10)); // debounce delay
            if (gpio_get_level(BUTTON_PIN) == 0) {
                press_count++;
                ESP_LOGI(Button_TAG, "Button pressed. Count: %d", press_count);
            }
        }
        gpio_set_level(LED_PIN, state);  // LED reflects current button state
        last_state = state;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void app_main(void) {
    ESP_LOGI(main_func, "GPIO Initialisation");
    // Configure LED GPIO
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Configure Button GPIO
    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    // Configure GPIO Potentiometer
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(POTENTIO_CHAN, ADC_ATTEN_DB_11);

    // Display configuration
    display_init();

    ESP_LOGI(main_func, "Tasks Initialisation");

    // Tasks
    xTaskCreate(display_task, "display_task", 2048, NULL, 2, NULL);
    xTaskCreate(button_task, "button_task", 2048, NULL, 2, NULL);
}
