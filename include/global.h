#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern float glob_temperature;
extern float glob_humidity;

// Separate queues for each consumer task
extern xQueueHandle tempQueue_LED;
extern xQueueHandle tempQueue_Server;
extern xQueueHandle tempQueue_CoreIOT;
extern xQueueHandle tempQueue_TinyML;

extern xQueueHandle humiQueue_Neo;
extern xQueueHandle humiQueue_Server;
extern xQueueHandle humiQueue_CoreIOT;
extern xQueueHandle humiQueue_TinyML;

extern QueueHandle_t lcdQueue;

// Semaphore for LED temperature synchronization
extern SemaphoreHandle_t Sema4need4LedBlinky_Temp;
extern SemaphoreHandle_t Sema4need4NeoBlinky_Humi;
extern SemaphoreHandle_t Sema4need4Humi;
extern SemaphoreHandle_t Sema4need4Temp;
extern SemaphoreHandle_t Sema4need4LCD;
extern SemaphoreHandle_t Sema4LCDtemp;
extern SemaphoreHandle_t Sema4LCDtempA;
extern SemaphoreHandle_t Sema4LCDtempB;
extern SemaphoreHandle_t Sema4LCDhumi;
extern SemaphoreHandle_t Sema4LCDhumiA;
extern SemaphoreHandle_t Sema4LCDhumiB;


extern String WIFI_SSID;
extern String WIFI_PASS;
extern String CORE_IOT_TOKEN;
extern String CORE_IOT_SERVER;
extern String CORE_IOT_PORT;

extern String ssid;
extern String password;
extern String wifi_ssid;
extern String wifi_password;
extern boolean isWifiConnected;
extern SemaphoreHandle_t xBinarySemaphoreInternet;
#endif