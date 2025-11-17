#include "led_blinky.h"

void led_blinky(void *pvParameters){
    // Initialize LED pin as output
    pinMode(LED_GPIO, OUTPUT);
    
    float temperature = 0;
    
    printf("[LED_BLINK] Task started - waiting for temperature data\n");
    
    while(1) {
        // Wait for semaphore signal from temp_humi_monitor task
        // This ensures synchronization between temperature reading and LED control
        if (xSemaphoreTake(Sema4need4LedBlinky, portMAX_DELAY) == pdTRUE) {
            if (xQueueReceive(tempQueue, &temperature, pdMS_TO_TICKS(500))==pdPASS){
                printf("[LED_BLINK] Receive Temperature: %f - Free: %d\n", temperature, uxQueueSpacesAvailable(tempQueue));
            } else {
                printf("[LED_BLINK] Failed to receive temperature.\n\n");
            }

            if (temperature <= 26){
                digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
                vTaskDelay(1000);
            }
            else if (temperature > 26 && temperature < 30){
                digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
                vTaskDelay(1000);
                digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
                vTaskDelay(1000);
            }
            else if (temperature >= 30){
                digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
                vTaskDelay(1000);
            }
        }
    }
}