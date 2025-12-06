#include "temp_humi_monitor.h"
DHT20 dht20;
LiquidCrystal_I2C lcd(0x21,16,2);

struct LCDData {
    float temperature;
    float humidity;
};


void temp_humi_monitor(void *pvParameters){

    Wire.begin(11, 12);
    Serial.begin(115200);
    dht20.begin();
    // lcd.begin();
    // lcd.backlight();


    while (1){
        /* code */
        
        dht20.read();
        // Reading temperature in Celsius
        float temperature = dht20.getTemperature();
        // if (temperature <= 26) xSemaphoreGive(Sema4LCDtemp);
        // else if (temperature > 26 && temperature < 30) xSemaphoreGive(Sema4LCDtempA);
        // else if (temperature >= 30) xSemaphoreGive(Sema4LCDtempB);
        // Reading humidity
        float humidity = dht20.getHumidity();
        // if (humidity < 40) xSemaphoreGive(Sema4LCDhumi);
        // else if (humidity >= 40 && humidity <= 60) xSemaphoreGive(Sema4LCDhumiA);
        // else if (humidity > 60) xSemaphoreGive(Sema4LCDhumiB);

        

        // Check if any reads failed and exit early
        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read from DHT sensor!");
            temperature = humidity =  -1;
            //return;
        }


        // Send temperature to all consumer queues
        xQueueSend(tempQueue_LED, &temperature, 0);
        xQueueSend(tempQueue_Server, &temperature, 0);
        xQueueSend(tempQueue_CoreIOT, &temperature, 0);
        xQueueSend(tempQueue_TinyML, &temperature, 0);
        printf("Send Temperature: %.2f°C\n", temperature);

        // Send humidity to all consumer queues
        xQueueSend(humiQueue_Neo, &humidity, 0);
        xQueueSend(humiQueue_Server, &humidity, 0);
        xQueueSend(humiQueue_CoreIOT, &humidity, 0);
        xQueueSend(humiQueue_TinyML, &humidity, 0);
        printf("Send Humidity: %.2f%%\n", humidity);


        // Display directly on LCD
        // lcd.clear();
        // lcd.setCursor(0, 0);
        // lcd.print("H:");
        // if (xSemaphoreTake(Sema4LCDhumi, pdMS_TO_TICKS(500))) lcd.print(" Low");
        // else if (xSemaphoreTake(Sema4LCDhumiA, pdMS_TO_TICKS(500))) lcd.print(" Norm");
        // else if (xSemaphoreTake(Sema4LCDhumiB, pdMS_TO_TICKS(500))) lcd.print(" High");

        // lcd.setCursor(0, 1);
        // lcd.print("T:");
        // if (xSemaphoreTake(Sema4LCDtemp, pdMS_TO_TICKS(500))) lcd.print(" Cold");
        // else if (xSemaphoreTake(Sema4LCDtempA, pdMS_TO_TICKS(500))) lcd.print(" Norm");
        // else if (xSemaphoreTake(Sema4LCDtempB, pdMS_TO_TICKS(500))) lcd.print(" Hot");

        // Print the results
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.print("%  Temperature: ");
        Serial.print(temperature);
        Serial.println("°C");
        
        vTaskDelay(5000);
    }
    
}