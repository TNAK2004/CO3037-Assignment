#include "neo_blinky.h"


void neo_blinky(void *pvParameters){

    Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    // Set all pixels to off to start
    strip.clear();
    strip.show();

    float humidity = 0;
    while(1) {
        if (xQueueReceive(humiQueue, &humidity, pdMS_TO_TICKS(500))==pdPASS){
            printf("Receive Humidity: %f - Free: %d\n", humidity, uxQueueSpacesAvailable(humiQueue));
        } else {
            printf("Failed to receive humidity.\n\n");
        }
        
        strip.setPixelColor(0, strip.Color(255, 255, 0)); // Set pixel 0 to yellow
        strip.show(); // Update the strip

        // Wait for 500 milliseconds
        vTaskDelay(500);

        // Set the pixel to off
        strip.setPixelColor(0, strip.Color(0, 0, 0)); // Turn pixel 0 off
        strip.show(); // Update the strip

        // Wait for another 500 milliseconds
        vTaskDelay(500);
    }
}