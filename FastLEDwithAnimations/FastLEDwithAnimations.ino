#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "wifi_name_here"
#define STAPSK  "wifi_password"
#endif

#define NUM_LEDS 60
#define LED_PIN 2

ESP8266WebServer server(80);
CRGB led[NUM_LEDS];
CRGB background[NUM_LEDS];

const char* ssid = STASSID;
const char* password = STAPSK;
uint8_t ledState = 0;
uint8_t hue = 0;
uint8_t pi1 = 0;
uint8_t ledon = 0;

CRGBPalette16 firePalette = HeatColors_p;

DEFINE_GRADIENT_PALETTE( Sunset_Real_gp ) {
    0, 120,  0,  0,
   22, 179, 22,  0,
   51, 255,104,  0,
   85, 167, 22, 18,
  135, 100,  0,103,
  198,  16,  0,130,
  255,   0,  0,160};

CRGBPalette16 myPal = Sunset_Real_gp;

CRGBPalette16 purplePalette = CRGBPalette16 (
    CRGB::DarkViolet,
    CRGB::DarkViolet,
    CRGB::DarkViolet,
    CRGB::DarkViolet,
    
    CRGB::Magenta,
    CRGB::Magenta,
    CRGB::Linen,
    CRGB::Linen,
    
    CRGB::Magenta,
    CRGB::Magenta,
    CRGB::DarkViolet,
    CRGB::DarkViolet,

    CRGB::DarkViolet,
    CRGB::DarkViolet,
    CRGB::Linen,
    CRGB::Linen
);

CRGBPalette16 myPalPurple = purplePalette;

uint8_t pi2 = 0;

DEFINE_GRADIENT_PALETTE (heatmap_gp) {
    0,   0,   0,   0,   //black
  128, 255,   0,   0,   //red
  200, 255, 255,   0,   //bright yellow
  255, 255, 255, 255    //full white 
};

CRGBPalette16 myPalHeat = heatmap_gp;

CRGBPalette16  lavaPalette = CRGBPalette16(
  CRGB::DarkRed,  CRGB::Maroon,   CRGB::DarkRed,  CRGB::Maroon,
  CRGB::DarkRed,  CRGB::Maroon,   CRGB::DarkRed,  CRGB::DarkRed,
  CRGB::DarkRed,  CRGB::DarkRed,  CRGB::Red,      CRGB::Orange,
  CRGB::White,    CRGB::Orange,   CRGB::Red,      CRGB::DarkRed
);

uint16_t brightnessScale = 150;
uint16_t indexScale = 100;

void handleRoot() {
  String message = "<html><head></head><body style='font-size: 12px'>Following functions are available:<br><br>";
  message += "<a href='/led?s=4'>WAVE</a><br>";
  message += "<a href='/led?s=5'>FIRE</a><br>";
  message += "<a href='/led?s=6'>RAINBOW BEAT</a><br>";
  message += "<a href='/led?s=7'>PHASE BEAT</a><br>";
  message += "<a href='/led?s=8'>MOVING DOT</a><br>";
  message += "<a href='/led?s=9'>BLUR PHASE BEAT</a><br>";
  message += "<a href='/led?s=10'>SAWTOOTH</a><br>";
  message += "<a href='/led?s=11'>PALETTE</a><br>";
  message += "<a href='/led?s=12'>TWINKLE</a><br>";
  message += "<a href='/led?s=13'>MOVING PIXEL</a><br>";
  message += "<a href='/led?s=14'>LAVA</a><br>";
  message += "<a href='/led?s=15'>PRETTY FILL</a><br>";
  message += "<a href='/led?s=16'>CLOCK</a><br>";
  message += "<a href='/led'>SWITCH OFF</a><br>";
  message += "<br/><br/>Version 2.0<br>";
  server.send(200, "text/html", message);
}

void handleNotFound() {
  //digitalWrite(led, 1);
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
  //digitalWrite(led, 0);
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
  FastLED.addLeds<WS2812, LED_PIN, GRB>(led, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.clear();
  FastLED.show();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
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
    /*case 0: digitalWrite(LED_BUILTIN, LOW);
      break;
    case 1: digitalWrite(LED_BUILTIN, HIGH);
      break;
    case 2: blinker();
      break;
    case 3: fast();
      break;*/
    case 4: wave();
      break;
    case 5: runFire();
      break;
    case 6: rainbowBeat();
      break;
    case 7: phaseBeat();
      break;
    case 8: movingDot();
      break;
    case 9: blurPhaseBeat();
      break;
    case 10: sawtooth();
      break;
    case 11: palette();
      break;
    case 12: fadeToBlack();
      break;
    case 13: movingPixel();
      break;
    case 14: lava();
      break;
    case 15: prettyFill();
      break;
    case 16: clockWork();
      break;
    default: switchOff();
      break;
  }
}

int getArgValue(String name) {
  for (uint8_t i = 0; i < server.args(); i++)
    if(server.argName(i) == name)
      return server.arg(i).toInt();
  return -1;
}

void runFire() {
  EVERY_N_MILLISECONDS(5) {
    int  a = millis();
    
    for (int i = 0; i < NUM_LEDS; i++) {
  
      // 3D noise, x is constant, we move through time on y and z axis
      // The 60 here will change the scale of the effect, lower is smoother
      // higher is more flickery. The time value for z was in the original code
      // as that was a 2D matrix version. I've left it in here as it looks 
      // nice in 1D too!
      uint8_t noise = inoise8 (0 , i * 60 + a , a / 3);
  
      // Divides 255 by (NUM_LEDS - 1) and subtracts that amount away from 255 to return 
      // a decreasing number each time e.g. for NUM_LEDS = 18, difference between
      // each point is 15, so:
      // i = 0, math = 255
      // i = 1, math = 240
      // i = 2, math = 225
      // ...
      // i = NUM_LEDS, math =  0
      uint8_t math = abs8(i - (NUM_LEDS-1)) * 255 / (NUM_LEDS-1);
  
      // Take the noise function from earlier and subtract math from it,
      // so the further along the strip you are, the higher palette index you
      // are likely to get. This results in the base of the fire (white, yellow)
      // being at the far end of the strip
      uint8_t index = qsub8 (noise, math);
  
      // Set the LED color from the palette
      led[i] = ColorFromPalette (firePalette, index, 255);    
    }
  }
  FastLED.show();
}

void wave() {
  for (int i=0; i<NUM_LEDS; i++){
    led[i] = CRGB(0, 255 - 4*i, 4*i );
    FastLED.setBrightness(2*i);
    FastLED.show();
    delay(50);
  }
  // Turn lights from blue to magenta from right to left 
  for (int i=NUM_LEDS; i>0; i--){
    led[i] = CRGB(4*i,0 , 255-4*i);
    FastLED.setBrightness(100-i);
    FastLED.show();
    delay(50);
  }
}

void rainbowBeat() {
    uint16_t beatA = beatsin16(30, 0, 255);
    uint16_t beatB = beatsin16(20, 0, 255);
    fill_rainbow(led, NUM_LEDS, (beatA+beatB)/2, 8);
    FastLED.show();
}

void sawtooth() {
  uint8_t pos = map(beat16(40, 0), 0, 65535, 0, NUM_LEDS -1);
  led[pos] = CHSV( hue, 200, 255);

  fadeToBlackBy(led, NUM_LEDS, 3);

  EVERY_N_MILLISECONDS( 10 ) {
    hue++;
  }
  
  FastLED.show();
}

void phaseBeat() {

  uint8_t sinBeat   = beatsin8(30, 0, NUM_LEDS - 1, 0, 0);
  uint8_t sinBeat2  = beatsin8(30, 0, NUM_LEDS - 1, 0, 85);
  uint8_t sinBeat3  = beatsin8(30, 0, NUM_LEDS - 1, 0, 170);

  // If you notice that your pattern is missing out certain LEDs, you
  // will need to use the higher resolution beatsin16 instead. In this
  // case remove the 3 lines above and replace them with the following:
  // uint16_t sinBeat   = beatsin16(30, 0, NUM_LEDS - 1, 0, 0);
  // uint16_t sinBeat2  = beatsin16(30, 0, NUM_LEDS - 1, 0, 21845);
  // uint16_t sinBeat3  = beatsin16(30, 0, NUM_LEDS - 1, 0, 43690);

  led[sinBeat]   = CRGB::Blue;
  led[sinBeat2]  = CRGB::Red;
  led[sinBeat3]  = CRGB::White;
  
  fadeToBlackBy(led, NUM_LEDS, 10);

  FastLED.show();
}

void movingDot() {
  uint16_t sinBeat = beatsin16(30, 0, NUM_LEDS - 1, 0, 0);

  led[sinBeat] = CRGB::Blue;
  
  fadeToBlackBy(led, NUM_LEDS, 10);

  EVERY_N_MILLISECONDS(10){
    Serial.println(sinBeat);
  }

  FastLED.show();
}

void blurPhaseBeat() {

  uint8_t sinBeat   = beatsin8(30, 0, NUM_LEDS - 1, 0, 0);
  uint8_t sinBeat2  = beatsin8(30, 0, NUM_LEDS - 1, 0, 85);
  uint8_t sinBeat3  = beatsin8(30, 0, NUM_LEDS - 1, 0, 170);

  // If you notice that your pattern is missing out certain LEDs, you
  // will need to use the higher resolution beatsin16 instead. In this
  // case remove the 3 lines above and replace them with the following:
  // uint16_t sinBeat   = beatsin16(30, 0, NUM_LEDS - 1, 0, 0);
  // uint16_t sinBeat2  = beatsin16(30, 0, NUM_LEDS - 1, 0, 21845);
  // uint16_t sinBeat3  = beatsin16(30, 0, NUM_LEDS - 1, 0, 43690);

  led[sinBeat]   = CRGB::Green;
  led[sinBeat2]  = CRGB::Blue;
  led[sinBeat3]  = CRGB::Red;

  EVERY_N_MILLISECONDS(1){
    for(int i = 0; i < 4; i++) {
      blur1d(led, NUM_LEDS, 50);
    }
  }

  FastLED.show();
}

void palette() {
  fill_palette(led, NUM_LEDS, pi1, 255 / NUM_LEDS, myPal, 255, LINEARBLEND);
  
  EVERY_N_MILLISECONDS(10){
    pi1++;
  }
  
  FastLED.show();
}

void fadeToBlack() {
  EVERY_N_MILLISECONDS(50){
    //Switch on an LED at random, choosing a random color from the palette
    led[random8(0, NUM_LEDS - 1)] = ColorFromPalette(myPalPurple, random8(), 255, LINEARBLEND);
  }

   // Fade all LEDs down by 1 in brightness each time this is called
   fadeToBlackBy(led, NUM_LEDS, 1);
  
  FastLED.show();
}

void heatMap() {
  fill_palette(led, NUM_LEDS, pi2, 255 / NUM_LEDS, myPalHeat, 255, LINEARBLEND);
  
  EVERY_N_MILLISECONDS(10){
    pi2++;
  }
  
  FastLED.show();
}

void drawBackground() {
  // A simple plasma effect
  fill_noise16 (background, NUM_LEDS, 1, millis(), 30, 1, 0, 50, millis() / 3, 10);
}

void drawMovingPixel() {
  // A pixel that moves back and forth using noise
  uint16_t pos = inoise16(millis() * 100);
  pos = constrain(pos, 13000, 51000);
  pos = map(pos, 13000, 51000, 0, NUM_LEDS - 1);
  led[pos] = CRGB::Red;
}

void movingPixel() {
  drawBackground();
  drawMovingPixel();

  EVERY_N_MILLISECONDS(20) {
    fadeToBlackBy(led, NUM_LEDS, 10); 
    nblend(led, background, NUM_LEDS, 30);
  } 
  
  FastLED.show();
}

void lava() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t brightness = inoise8(i * brightnessScale, millis() / 5);
    uint8_t index = inoise8(i * indexScale, millis() /10);
    led[i] = ColorFromPalette(lavaPalette, index, brightness);
  }

  FastLED.show();
}

void prettyFill() {
  uint8_t octaves = 1;
  uint16_t x = 0;
  int scale = 100;
  uint8_t hue_octaves = 1;
  uint16_t hue_x = 1;
  int hue_scale = 50;
  uint16_t ntime = millis() / 3;
  uint8_t hue_shift = 5;
  
  fill_noise16 (led, NUM_LEDS, octaves, x, scale, hue_octaves, hue_x, hue_scale, ntime, hue_shift);
  FastLED.show();
}

void switchOff() {
  fadeToBlackBy(led, NUM_LEDS, 10);
  FastLED.show();
}

void clockWork() {
  if (ledon == 0)
    for (int i = 0; i < NUM_LEDS; i++) {
      led[i] = CRGB(255, 255, 100);
      delay(1000);
      FastLED.show();
    }
  else {
    for (int i = 0; i < NUM_LEDS; i++) {
      led[i] = CRGB(0, 0, 0);
      delay(1000);
      FastLED.show();
    }
  }
  if (ledon == 0) ledon = 1;
  else ledon = 0;
}
