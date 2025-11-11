#include "temp_humi_monitor.h"
DHT20 dht20;
LiquidCrystal_I2C lcd(33,16,2);


void temp_humi_monitor(void *pvParameters){

    Wire.begin(11, 12);
    Serial.begin(115200);
    dht20.begin();

    while (1){
        /* code */
        
        dht20.read();
        // Reading temperature in Celsius
        float temperature = dht20.getTemperature();
        // Reading humidity
        float humidity = dht20.getHumidity();

        

        // Check if any reads failed and exit early
        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read from DHT sensor!");
            temperature = humidity =  -1;
            //return;
        }

        // Send temperature to queue for LED task
        if (xQueueSend(tempQueue, &temperature, pdMS_TO_TICKS(500))==pdPASS){
            printf("Send Temperature: %.2f°C - Queue Free Spaces: %d\n", 
                   temperature, uxQueueSpacesAvailable(tempQueue));
            
            // Give semaphore to signal LED task that new temperature data is available
            // This provides synchronization between temp monitoring and LED control
            xSemaphoreGive(Sema4need4LedBlinky);
            printf("Semaphore given to theBLinkBlinkOneTask\n");
        } else {
            printf("Failed to send temperature to queue.\n");
        }

        if (xQueueSend(humiQueue, &humidity, pdMS_TO_TICKS(500))==pdPASS){
            printf("Send Humidity: %f - Free: %d\n", humidity, uxQueueSpacesAvailable(humiQueue));
        } else {
            printf("Failed to send humidity to queue.\n");
        }

        //Update global variables for temperature and humidity
        glob_temperature = temperature;
        glob_humidity = humidity;

        // Print the results
        
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.print("%  Temperature: ");
        Serial.print(temperature);
        Serial.println("°C");
        
        vTaskDelay(5000);
    }
    
}