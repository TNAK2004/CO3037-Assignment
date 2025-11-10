#include "led_blinky.h"

void led_blinky(void *pvParameters){
    pinMode(LED_GPIO, OUTPUT);
    float temperature = 0;
  while(1) {                        
      if (xQueueReceive(tempQueue, &temperature, pdMS_TO_TICKS(500))==pdPASS){
        printf("Receive Temperature: %f - Free: %d\n", temperature, uxQueueSpacesAvailable(tempQueue));
      } else {
        printf("Failed to receive temperature.\n\n");
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