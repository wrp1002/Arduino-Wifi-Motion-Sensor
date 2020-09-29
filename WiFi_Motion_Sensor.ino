#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


#define pirPin D5
#define buzzerPin D6


String ssid = "SSID";
String password = "PASSWORD";
String baseUrl = "http://raspberrypi.local:18089/";

// Create variables:
int pingInterval = 40;
int pingCount = 0;
bool motionState = false;

void WaitForWifi() {
  Serial.print("Connecting");
  
  bool ledOn = false;
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, ledOn);
    ledOn = !ledOn;
    Serial.print(".");
    delay(1000);
  }
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Done!");
}

void SendRequest(String endpoint) {
  String url = baseUrl + endpoint;
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
     
    http.begin(url);
    int httpCode = http.GET();
     
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
    }
     
    http.end();
  }
}

void Beep(int amount) {
	for (int i = 0; i < amount; i++) {
		digitalWrite(buzzerPin, HIGH);
		delay(100);
		digitalWrite(buzzerPin, LOW);
		delay(100);
	}
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println("Initializing...");
  pinMode(pirPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  WiFi.begin(ssid, password);
  WaitForWifi();
	Beep(2);
}

void loop() {
  if (digitalRead(pirPin) == HIGH) {
    if (motionState == false) {
      Serial.println("Motion detected!");
      Beep(5);
      SendRequest("motion");
      motionState = true;
    }
  }
  else
    motionState = false;

  pingCount++;
  if (pingCount >= pingInterval) {
    SendRequest("ping");
    pingCount = 0;
  }
  delay(500);
}
