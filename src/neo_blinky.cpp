#include "neo_blinky.h"


void neo_blinky(void *pvParameters){

    Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    // Set all pixels to off to start
    strip.clear();
    strip.show();

    float humidity = 0;
    while(1) {
        if (xSemaphoreTake(Sema4need4NeoBlinky_Humi, portMAX_DELAY)) {
            if (xQueueReceive(humiQueue, &humidity, pdMS_TO_TICKS(500))==pdPASS){
                printf("[NEO_BLINK] Receive Humidity: %f - Free: %d\n", humidity, uxQueueSpacesAvailable(humiQueue));
            } else {
                printf("[NEO_BLINK] Failed to receive humidity.\n\n");
            }
            
            if (humidity < 40){
                strip.setPixelColor(0, strip.Color(255, 0, 0)); // Set pixel 0 to red
                strip.show(); // Update the strip

                // Wait for 500 milliseconds
                vTaskDelay(500);
            } else if (humidity >= 40 && humidity <= 60){
                strip.setPixelColor(0, strip.Color(0, 255, 0)); // Set pixel 0 to green
                strip.show();
                vTaskDelay(500);
            } else if (humidity > 60){
                strip.setPixelColor(0, strip.Color(0, 0, 255)); // Set pixel 0 to blue
                strip.show();
                vTaskDelay(500);
            }
            // Set the pixel to off
            strip.setPixelColor(0, strip.Color(0, 0, 0)); // Turn pixel 0 off
            strip.show(); // Update the strip

            // Wait for another 500 milliseconds
            vTaskDelay(500);
        }

    }
}