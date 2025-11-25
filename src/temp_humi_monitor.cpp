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
    lcd.begin();
    lcd.backlight();


    while (1){
        /* code */
        
        dht20.read();
        // Reading temperature in Celsius
        float temperature = dht20.getTemperature();
        if (temperature <= 26) xSemaphoreGive(Sema4LCDtemp);
        else if (temperature > 26 && temperature < 30) xSemaphoreGive(Sema4LCDtempA);
        else if (temperature >= 30) xSemaphoreGive(Sema4LCDtempB);
        // Reading humidity
        float humidity = dht20.getHumidity();
        if (humidity < 40) xSemaphoreGive(Sema4LCDhumi);
        else if (humidity >= 40 && humidity <= 60) xSemaphoreGive(Sema4LCDhumiA);
        else if (humidity > 60) xSemaphoreGive(Sema4LCDhumiB);

        

        // Check if any reads failed and exit early
        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read from DHT sensor!");
            temperature = humidity =  -1;
            //return;
        }


        if (xQueueSend(tempQueue, &temperature, pdMS_TO_TICKS(500))==pdPASS){
            printf("Send Temperature: %.2f°C - Free: %d\n", 
                   temperature, uxQueueSpacesAvailable(tempQueue));
            
            // xSemaphoreGive(Sema4need4Temp);
            // xSemaphoreGive(Sema4need4LedBlinky_Temp);
            
        } else {
            printf("Failed to send temperature to queue.\n");
        }

        if (xQueueSend(humiQueue, &humidity, pdMS_TO_TICKS(500))==pdPASS){
            printf("Send Humidity: %f - Free: %d\n", humidity, uxQueueSpacesAvailable(humiQueue));

            // xSemaphoreGive(Sema4need4NeoBlinky_Humi);
            // xSemaphoreGive(Sema4need4Humi);
        } else {
            printf("Failed to send humidity to queue.\n");
        }


        // Display directly on LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("H:");
        if (xSemaphoreTake(Sema4LCDhumi, pdMS_TO_TICKS(500))) lcd.print(" Low");
        else if (xSemaphoreTake(Sema4LCDhumiA, pdMS_TO_TICKS(500))) lcd.print(" Norm");
        else if (xSemaphoreTake(Sema4LCDhumiB, pdMS_TO_TICKS(500))) lcd.print(" High");

        lcd.setCursor(0, 1);
        lcd.print("T:");
        if (xSemaphoreTake(Sema4LCDtemp, pdMS_TO_TICKS(500))) lcd.print(" Cold");
        else if (xSemaphoreTake(Sema4LCDtempA, pdMS_TO_TICKS(500))) lcd.print(" Norm");
        else if (xSemaphoreTake(Sema4LCDtempB, pdMS_TO_TICKS(500))) lcd.print(" Hot");

        // //Update global variables for temperature and humidity
        // glob_temperature = temperature;
        // glob_humidity = humidity;

        // Print the results
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.print("%  Temperature: ");
        Serial.print(temperature);
        Serial.println("°C");
        
        vTaskDelay(5000);
    }
    
}