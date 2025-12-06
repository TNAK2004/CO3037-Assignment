#include "coreiot.h"

// ----------- CONFIGURE THESE! -----------
const char* coreIOT_Server = "app.coreiot.io";  
const char* coreIOT_Token = "uiopBTyMUT1LEATqojUw";   // Device Access Token uiopBTyMUT1LEATqojUw
const int   mqttPort = 1883;
// ----------------------------------------

WiFiClient espClient;
PubSubClient client(espClient);


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect (username=token, password=empty)
    // Change only const char *id
    if (client.connect("ESP32Client", coreIOT_Token, NULL)) {
      Serial.println("connected to CoreIOT Server!");
      client.subscribe("v1/devices/me/rpc/request/+");
      Serial.println("Subscribed to v1/devices/me/rpc/request/+");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("]");

  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  Serial.print("Payload: ");
  Serial.println(message);

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.println("JSON parse failed!");
    return;
  }

  const char* method = doc["method"];
  JsonVariant params = doc["params"];

  // ----------------------------- SET LED -----------------------------
  if (strcmp(method, "setStateLED") == 0) {
    if (params == "ON" || params == 1) {
      digitalWrite(LED1_PIN, HIGH);
      Serial.println("LED → ON");
    } else {
      digitalWrite(LED1_PIN, LOW);
      Serial.println("LED → OFF");
    }

    client.publish("v1/devices/me/rpc/response/1",
                   "{\"result\":\"LED updated\"}");
  }

  // ----------------------------- SET FAN -----------------------------
  else if (strcmp(method, "setStateFAN") == 0) {
    if (params == "ON" || params == 1) {
      digitalWrite(FAN_PIN, HIGH);
      Serial.println("FAN → ON");
    } else {
      digitalWrite(FAN_PIN, LOW);
      Serial.println("FAN → OFF");
    }

    client.publish("v1/devices/me/rpc/response/1",
                   "{\"result\":\"FAN updated\"}");
  }

  // ----------------------------- GET LED STATE -----------------------------
  else if (strcmp(method, "getStateLED") == 0) {

    int state = digitalRead(LED1_PIN);

    String resp = String("{\"state\":") + (state == HIGH ? "\"ON\"" : "\"OFF\"") + "}";
    client.publish("v1/devices/me/rpc/response/1", resp.c_str());

    Serial.print("Return LED state: ");
    Serial.println(resp);
  }

  // ----------------------------- GET FAN STATE -----------------------------
  else if (strcmp(method, "getStateFAN") == 0) {

    int state = digitalRead(FAN_PIN);

    String resp = String("{\"state\":") + (state == HIGH ? "\"ON\"" : "\"OFF\"") + "}";
    client.publish("v1/devices/me/rpc/response/1", resp.c_str());

    Serial.print("Return FAN state: ");
    Serial.println(resp);
  }

  // ----------------------------- UNKNOWN -----------------------------
  else {
    Serial.print("Unknown method: ");
    Serial.println(method);
  }
}




void setup_coreiot(){

  //Serial.print("Connecting to WiFi...");
  //WiFi.begin(wifi_ssid, wifi_password);
  //while (WiFi.status() != WL_CONNECTED) {
  
  // while (isWifiConnected == false) {
  //   delay(500);
  //   Serial.print(".");
  // }

  while(1){
    if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY)) {
      break;
    }
    delay(500);
    Serial.print(".");
  }


  Serial.println(" Connected!");

  // Initialize LED pin
  pinMode(LED1_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(LED1_PIN, LOW);

  client.setServer(coreIOT_Server, mqttPort);
  client.setCallback(callback);

}

void coreiot_task(void *pvParameters){

    setup_coreiot();
    float temperature = 0;
    float humidity = 0;
  

    while(1){

        if (!client.connected()) {
            reconnect();
        }
        client.loop();


        if (xQueueReceive(humiQueue_CoreIOT, &humidity, pdMS_TO_TICKS(100))) {  
            printf("[CoreIOT] Receive Humidity: %.2f\n", humidity);
        }

        if (xQueueReceive(tempQueue_CoreIOT, &temperature, pdMS_TO_TICKS(100))) {  
            printf("[CoreIOT] Receive Temperature: %.2f\n", temperature);
        }

        // Sample payload, publish to 'v1/devices/me/telemetry'
        String payload = "{\"temperature\":" + String(temperature) +  ",\"humidity\":" + String(humidity) + "}";
        
        client.publish("v1/devices/me/telemetry", payload.c_str());

        Serial.println("Published payload: " + payload);
        vTaskDelay(10000);  // Publish every 10 seconds
    }
}