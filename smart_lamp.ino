#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <FastLED.h>

#define LED_PIN 4
#define NUM_LEDS 16
#define BUTTON_PIN 5

CRGB leds[NUM_LEDS];
WebServer server(80);

const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

bool isOn = false;
int brightness = 100;
CRGB color = CRGB::White;

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Умная лампа</h1>";
  html += "<p><a href=\"/on\">Включить</a> | <a href=\"/off\">Выключить</a></p>";
  html += "<p>Яркость: <a href=\"/brightness?value=50\">50%</a> | <a href=\"/brightness?value=100\">100%</a></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleOn() {
  isOn = true;
  updateLeds();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  isOn = false;
  updateLeds();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleBrightness() {
  brightness = server.arg("value").toInt();
  updateLeds();
  server.sendHeader("Location", "/");
  server.send(303);
}

void updateLeds() {
  if (isOn) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = color;
      leds[i].fadeLightBy(255 - brightness * 2.55);
    }
  } else {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP адрес: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/brightness", handleBrightness);
  server.begin();
}

void loop() {
  server.handleClient();

  if (digitalRead(BUTTON_PIN) == LOW) {
    isOn = !isOn;
    updateLeds();
    delay(300);
  }
}
