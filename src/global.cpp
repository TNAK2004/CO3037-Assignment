#include "global.h"
float glob_temperature = 0;
float glob_humidity = 0;
xQueueHandle tempQueue = xQueueCreate(5, sizeof(float));
xQueueHandle humiQueue = xQueueCreate(5, sizeof(float));

// Binary semaphore for temperature-LED synchronization (Task 1)
// SemaphoreHandle_t Sema4need4LedBlinky_Temp = xSemaphoreCreateBinary();
// SemaphoreHandle_t Sema4need4NeoBlinky_Humi = xSemaphoreCreateBinary();
// SemaphoreHandle_t Sema4need4Humi = xSemaphoreCreateBinary();
// SemaphoreHandle_t Sema4need4Temp = xSemaphoreCreateBinary();
// SemaphoreHandle_t Sema4need4LCD = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4LCDtemp = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4LCDtempA = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4LCDtempB = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4LCDhumi = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4LCDhumiA = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4LCDhumiB = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4LED = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4LEDA = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4LEDB = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4NEO = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4NEOA = xSemaphoreCreateBinary();
SemaphoreHandle_t Sema4NEOB = xSemaphoreCreateBinary();

String WIFI_SSID;
String WIFI_PASS;
String CORE_IOT_TOKEN;
String CORE_IOT_SERVER;
String CORE_IOT_PORT;

String ssid = "ESP32-NAAK";
String password = "12345678";
String wifi_ssid = "abcde";
String wifi_password = "123456789";
boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();