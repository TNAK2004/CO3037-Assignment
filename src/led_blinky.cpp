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
            
            // Attempt to receive temperature from queue with timeout
            if (xQueueReceive(tempQueue, &temperature, pdMS_TO_TICKS(500)) == pdPASS) {
                printf("[LED_BLINK] Received Temperature: %.2f°C - Queue Free Spaces: %d\n", 
                       temperature, uxQueueSpacesAvailable(tempQueue));
                
                /* 
                 * BEHAVIOR 1: COLD CONDITION (temp <= 24°C)
                 * LED blinks slowly to indicate cold temperature
                 * Blink pattern: 500ms ON, 500ms OFF
                 */
                if (temperature <= 24.0) {
                    printf("[LED_BLINK] COLD Mode: Slow blinking (temp <= 24°C)\n");
                    digitalWrite(LED_GPIO, HIGH);  // Turn LED ON
                    vTaskDelay(pdMS_TO_TICKS(500));
                    digitalWrite(LED_GPIO, LOW);   // Turn LED OFF
                    vTaskDelay(pdMS_TO_TICKS(500));
                }
                /* 
                 * BEHAVIOR 2: NORMAL CONDITION (24°C < temp < 28°C)
                 * LED stays ON continuously to indicate comfortable temperature
                 */
                else if (temperature > 24.0 && temperature < 28.0) {
                    printf("[LED_BLINK] NORMAL Mode: LED ON (24°C < temp < 28°C)\n");
                    digitalWrite(LED_GPIO, HIGH);  // Keep LED ON
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
                /* 
                 * BEHAVIOR 3: HOT CONDITION (temp >= 28°C)
                 * LED blinks rapidly to indicate hot temperature (warning)
                 * Blink pattern: 200ms ON, 200ms OFF
                 */
                else { // temperature >= 28.0
                    printf("[LED_BLINK] HOT Mode: Fast blinking (temp >= 28°C)\n");
                    digitalWrite(LED_GPIO, HIGH);  // Turn LED ON
                    vTaskDelay(pdMS_TO_TICKS(200));
                    digitalWrite(LED_GPIO, LOW);   // Turn LED OFF
                    vTaskDelay(pdMS_TO_TICKS(200));
                }
                
            } else {
                printf("[LED_BLINK] Failed to receive temperature from queue.\n");
                // Turn off LED if no data received
                digitalWrite(LED_GPIO, LOW);
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
    }
}