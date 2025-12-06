#include "led_blinky.h"

void led_blinky(void *pvParameters){
    // Initialize LED pin as output
    pinMode(LED_GPIO, OUTPUT);
    
    float temperature = 0;
    
    printf("[LED_BLINK] Task started - waiting for temperature data\n");
    
    while(1) {
            if (xQueueReceive(tempQueue_LED, &temperature, pdMS_TO_TICKS(100))){
                printf("[LED_BLINK] Receive Temperature: %.2f\n", temperature);
            }

            if (temperature <= 26){
                digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
                vTaskDelay(1000);
            }
            else if (temperature > 26 && temperature < 30){
                digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
                vTaskDelay(500);
                digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
                vTaskDelay(500);
            }
            else if (temperature >= 30){
                digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
                vTaskDelay(1000);
            }
        // }
    }
}