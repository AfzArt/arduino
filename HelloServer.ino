#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "wifi_name_here"
#define STAPSK  "wifi_password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
uint8_t ledState = 0;

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

void handleRoot() {
  String message = "<html><head></head><body style='font-size: 12px'>Following functions are available:<br><br>";
  message += "<a href='/led?s=0'>LED ON</a> Switches on the ON-BOARD LED<br>";
  message += "<a href='/led?s=1'>LED OFF</a> Switches off the ON-BOARD LED<br>";
  message += "<a href='/led?s=2'>Blink</a> Makes the LED blink<br>";
  message += "<a href='/led?s=3'>Faster</a> Makes the LED blinks faster<br>";
  message += "<br/><br/>Version 0.6<br>";
  server.send(200, "text/html", message);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void blinker() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);                      
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
}

void fast() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/led", []() {
    server.send(200, "text/plain", "LED state received!");
    ledState = getArgValue("s");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  switch (ledState) {
    case 0: digitalWrite(LED_BUILTIN, LOW);
      break;
    case 1: digitalWrite(LED_BUILTIN, HIGH);
      break;
    case 2: blinker();
      break;
    case 3: fast();
      break;
    default: digitalWrite(LED_BUILTIN, HIGH);
      break;
  }
}

int getArgValue(String name) {
  for (uint8_t i = 0; i < server.args(); i++)
    if(server.argName(i) == name)
      return server.arg(i).toInt();
  return -1;
}
