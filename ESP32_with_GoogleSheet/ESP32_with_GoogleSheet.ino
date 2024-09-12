#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "DHT.h"

#define DHTPIN 4 // Pin connected to DHT11
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Button pins
#define BUTTON_SEND_PIN 2 // Button 1 for sending data
#define BUTTON_STATE_PIN 16 // Button 2 for recording state


// Replace with your network credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Google Apps Script Web App URL
const char* serverName = "URL";

// Initialize WiFi and NTP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000); // Update every 60 seconds

void setup() {
  Serial.begin(115200);

  // Setup WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Start DHT sensor
  dht.begin();

  // Initialize NTP client
  timeClient.begin();
  timeClient.update();

  // Setup buttons as input
  pinMode(BUTTON_SEND_PIN, INPUT_PULLUP);  // Button 1 for sending data
  pinMode(BUTTON_STATE_PIN, INPUT_PULLUP); // Button 2 for state tracking
}

void loop() {
  // Check if the send button is pressed
 if (digitalRead(BUTTON_SEND_PIN) == LOW) {
  //   // Get the state of the second button (pressed/not pressed)
     bool buttonState = digitalRead(BUTTON_STATE_PIN) == LOW ? true : false;

    if (WiFi.status() == WL_CONNECTED) { // Check if ESP32 is connected to WiFi
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");

      // Fetch time from NTP client
      timeClient.update();
      String formattedTime = timeClient.getFormattedTime();

      // Get DHT11 sensor data
      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();

      // Prepare JSON payload with NTP time, sensor data, and button state
      String jsonData = "{\"method\":\"append\", \"temperature\":" + String(temperature) +
                        ", \"humidity\":" + String(humidity) +
                        ", \"timestamp\":\"" + formattedTime + "\"" +
                        ", \"buttonState\":" + String(buttonState ? "true" : "false") + "}";

      // Send HTTP POST request
      int httpResponseCode = http.POST(jsonData);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      } else {
        Serial.println("Error on sending POST: " + String(httpResponseCode));
      }

      http.end(); // Close connection
    } else {
      Serial.println("WiFi Disconnected");
    }

    delay(500); // Debounce delay for the button
   }

  delay(1000); // General loop delay
}
