#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_PIN GPIO_NUM_23
#define LED_PIN    GPIO_NUM_2

static const char *TAG = "BUTTON_LED_APP";

void app_main(void)
{
    // Configure LED GPIO
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Configure Button GPIO
    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);        
    
    char state[20];

    while(1) {
        int button_state = !gpio_get_level(BUTTON_PIN);

        gpio_set_level(LED_PIN, button_state);
        
        snprintf(state, sizeof(state), "LED State : %d", button_state);

        ESP_LOGI(TAG, "%s", state);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
