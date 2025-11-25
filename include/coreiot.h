#ifndef __COREIOT_H__
#define __COREIOT_H__

#include <Arduino.h>
#include <WiFi.h>
#include "global.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

// LED Pin definition for CoreIOT control
#define LED1_PIN 48
#define FAN_PIN 8

void coreiot_task(void *pvParameters);

#endif