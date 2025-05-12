#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_PIN GPIO_NUM_32
#define LED_PIN    GPIO_NUM_23

static const char *TAG = "BUTTON_LED_APP";

void app_main(void)
{
    // Configure LED pin as output
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Configure Button pin as input with pull-up
    io_conf.pin_bit_mask = (1ULL << BUTTON_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    while (1) {
        int button_state = gpio_get_level(BUTTON_PIN);
        ESP_LOGI(TAG, "Button State: %d", button_state);

        if (button_state == 0) {
            gpio_set_level(LED_PIN, 0);  // Turn LED on
        } else {
            gpio_set_level(LED_PIN, 1);  // Turn LED off
        }

        vTaskDelay(pdMS_TO_TICKS(100));  // Delay to debounce
    }
}
