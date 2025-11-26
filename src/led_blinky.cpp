#include "led_blinky.h"

void led_blinky(void *pvParameters){
    // Initialize LED pin as output
    pinMode(LED_GPIO, OUTPUT);
    
    float temperature = 0;
    
    printf("[LED_BLINK] Task started - waiting for temperature data\n");
    
    while(1) {
        // if (xSemaphoreTake(Sema4need4LedBlinky_Temp, portMAX_DELAY)) {
            // if (xQueueReceive(tempQueue, &temperature, pdMS_TO_TICKS(500))==pdPASS){
            //     printf("[LED_BLINK] Receive Temperature: %f - Free: %d\n", temperature, uxQueueSpacesAvailable(tempQueue));
            // } else {
            //     printf("[LED_BLINK] Failed to receive temperature.\n\n");
            // }

            if (xSemaphoreTake(Sema4LED, pdMS_TO_TICKS(500))){
                digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
                vTaskDelay(1000);
            }
            else if (xSemaphoreTake(Sema4LEDA, pdMS_TO_TICKS(500))){
                digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
                vTaskDelay(500);
                digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
                vTaskDelay(500);
            }
            else if (xSemaphoreTake(Sema4LEDB, pdMS_TO_TICKS(500))){
                digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
                vTaskDelay(1000);
            }
        // }
    }
}