#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "Sch!";
const char* password = "passxxxx";

const char* serverName = "http://xxxx.com/esp-outputs-action.php?action=outputs_state&board=1";

String outputsState;

unsigned long previousMillis = 0;
const unsigned long interval = 2000; // 2 seconds interval

void setup() {
  Serial.begin(115200);

  connectToWiFi(); // Connect to WiFi network
}

void loop() {
  unsigned long currentMillis = millis();

  // Check WiFi connection status and reconnect if necessary
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  // Check if it's time to make the request
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    outputsState = httpGETRequest(serverName);

    JSONVar obj = JSON.parse(outputsState);

    if (JSON.typeof(obj) == "undefined") {
      Serial.println("Parsing input failed!");
    }
    else {
      JSONVar keys = obj.keys();
      Serial.print("JSON object = ");
      Serial.println(obj);

      for (int i = 0; i < keys.length(); i++) {
        JSONVar value = obj[keys[i]];
        Serial.print("GPIO: ");
        Serial.print(keys[i]);
        Serial.print(" - SET to: ");
        Serial.println(value);
        pinMode(atoi(keys[i]), OUTPUT);
        digitalWrite(atoi(keys[i]), atoi(value));
      }
    }
  }
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);

  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();

  return payload;
}
