#include <FastLED.h>              // https://github.com/FastLED/FastLED
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define NUMLEDS 10               // Number of LEDs
#define DATA_PIN 19                // Connect your addressable LED strip to this pin.
#define SENSITIVITY 1000           // Ranges from 0 to 1023
#define MAX_BRIGHTNESS 255        // Ranges from 0 to 255
#define SOUND_PIN 34           // Connect sound detector to this pin
#define SATURATION 255            // Ranges from 0 to 255
#define MINVAL 0
#define HUE_INIT 95
#define HUE_CHANGE 0

const char* ssid = "***";
const char* password = "***";

const char* serverName = "http://api.thingspeak.com/update?api_key=***";
String apiKey = "***";
CRGB leds[NUMLEDS];
byte brightness[NUMLEDS];
byte hue[NUMLEDS];
int analogVal;
int DELAY;
int loud;
int beep;
int send_data;
int sensorNum = 1;
String hname = "Loudness_sensor-" + String(sensorNum);
const char* hostname = hname.c_str();
unsigned long previousMillis = 0;
unsigned long interval = 30000;

void setup() { 
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //   Serial.println("Connection Failed! Rebooting...");
  //   delay(5000);
  //   ESP.restart();
  // }

  // Port defaults to 3232
//  ArduinoOTA.setPort(8080);

  // Hostname defaults to esp3232-[MAC]
   ArduinoOTA.setHostname(hostname);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(SOUND_PIN, INPUT);
  pinMode(33, OUTPUT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUMLEDS);

//  for(int i = 0; i <= NUMLEDS; i++){
//    brightness[i] = 0;
//    hue[i] = 0;
//  }
//
//  //Turn off all the LEDs
//  for(int i=0; i <= NUMLEDS; i++)  
//  {
//  leds[i] = CRGB::Black;
//  }
//
//  //Update the LED strip
//  FastLED.show(); 
}

void loop() {
  ArduinoOTA.handle();

unsigned long currentMillis = millis();
 if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
   Serial.println("Reconnecting to WiFi...");
   WiFi.disconnect();
   WiFi.reconnect();   
   previousMillis = currentMillis;
 }
  
  for(int i = 0; i <= 50; i++)
  {
      analogVal += analogRead(SOUND_PIN);
  }
  int average = analogVal/50;
  analogVal = abs(average - analogRead(SOUND_PIN));

  int dBValue = average / 34;

  if(analogVal > SENSITIVITY)
  analogVal = SENSITIVITY;
  

  if(analogVal < MINVAL)
  analogVal = 0;

  if(send_data == 500)
  {
  Serial.println("Sending");
  
  WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "api_key=" + apiKey + "&field" + String(sensorNum) + "=" + String(dBValue);           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
    
    Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
  send_data = 0;
  }
  send_data += 1;

/*----------------------------------------------------------
  - 5 styles for sound reactive led strip are given below in different lines.
  - Uncomment the function which you want to try and comment the others.
  - "LinearFlowing" is uncommented by default.          
-----------------------------------------------------------*/

  // LinearFlowing();
   LinearReactive();
  // BrightnessReactive();
  // CentreProgressive();
  // EdgeProgressive();
  
  FastLED.show();
  delay(10);
}

void LinearFlowing(){
  byte val = map(analogVal, 0, SENSITIVITY+1, 0, MAX_BRIGHTNESS);

  
  for(int i = 0; i <= NUMLEDS; i++){
    brightness[NUMLEDS-i] = brightness[NUMLEDS-i-1];
  }
  
  for(int i = 0; i <= NUMLEDS; i++){
    hue[NUMLEDS-i] = hue[NUMLEDS-i-1];
  }
  
  brightness[0] = val;
  byte hue = HUE_INIT;
  for(int i = 0; i <= NUMLEDS; i++){
    if(i <= 19)
    {
      leds[i] = CHSV(95, 255, brightness[i]);
    }
    else
    {
      leds[i] = CHSV(0, 255, brightness[i]);
    }
  }
  delay(DELAY);
}

void LinearReactive(){
  byte val = map(analogVal, 0, SENSITIVITY+1, 0, NUMLEDS);
  byte hue = HUE_INIT;

  if(val >= 7)
  {
    loud++;
  }
  else
  {
    loud=0;
  }
  
  if(loud >= 4)
  {
    for(int i = 0; i <= 4; i++)
    {
        digitalWrite(33, HIGH);
        delay(100);
    }
    fill_solid(leds, NUMLEDS, CRGB::Red);
    FastLED.show();
    delay(100);
    fill_solid(leds, NUMLEDS, CRGB::Black);
    FastLED.show();
    delay(100);
    fill_solid(leds, NUMLEDS, CRGB::Red);
    FastLED.show();
    delay(100);
    fill_solid(leds, NUMLEDS, CRGB::Black);
    FastLED.show();
    delay(100);
    fill_solid(leds, NUMLEDS, CRGB::Red);
    FastLED.show();
  }
  else
  {
    digitalWrite(33, LOW); 
    for(int i = 0; i <= val; i++)
    {
      if(i <= 5)
      {
        leds[i] = CHSV(95, SATURATION, MAX_BRIGHTNESS);
      }
      else if(i>=6 && i<=7)
      {
        leds[i] = CHSV(64, SATURATION, MAX_BRIGHTNESS);
      }
      else if(i>=7)
      {
        leds[i] = CHSV(0, SATURATION, MAX_BRIGHTNESS);
      }
    } 
  }

  for(int i = val+1; i <= NUMLEDS; i++){
    leds[i].nscale8(10);
  }
}

void BrightnessReactive(){
  byte val = map(analogVal, 0, SENSITIVITY+1, 0, MAX_BRIGHTNESS);
  byte hue = HUE_INIT;
  for(int i = 0; i <= NUMLEDS; i++){
    leds[i] = CHSV(hue += HUE_CHANGE, SATURATION, val);
  }
}

void CentreProgressive(){
  byte val = map(analogVal, 0, SENSITIVITY, 0, NUMLEDS/2);
  byte hue = HUE_INIT;
  for(int i = 0; i <= val; i++){
    leds[(NUMLEDS/2)+i] = CHSV(hue += HUE_CHANGE, SATURATION, MAX_BRIGHTNESS);
    leds[(NUMLEDS/2)-i] = CHSV(hue += HUE_CHANGE, SATURATION, MAX_BRIGHTNESS);
  }

  for(int i = val+1; i <= (NUMLEDS/2); i++){
    leds[(NUMLEDS/2)+i].nscale8(10);
    leds[(NUMLEDS/2)-i].nscale8(10);
  }
}

void EdgeProgressive(){
  byte val = map(analogVal, 0, SENSITIVITY, 0, NUMLEDS/2);
  byte hue = HUE_INIT;
  for(int i = 0; i <= val; i++){
    leds[i] = CHSV(hue += HUE_CHANGE, SATURATION, MAX_BRIGHTNESS);
    leds[NUMLEDS-i] = CHSV(hue += HUE_CHANGE, SATURATION, MAX_BRIGHTNESS);
  }

  for(int i=val+1; i<=(NUMLEDS/2); i++){
    leds[i].nscale8(10);
    leds[NUMLEDS-i].nscale8(10);
  }
}
