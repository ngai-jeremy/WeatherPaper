#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <GxEPD2_BW.h>
#include "bitmaps.h"
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include "Petme8x8.h"
#include <Preferences.h>
#include <WiFiManager.h>
#include "qrcode.h"

#define batteryAdcPin 3
#define batteryChargingPin 2

// IP GEOLOCATION
String lat = "";
String lon = "";

// OPENWEATHERMAP API
char apiKey[34] = "";
bool shouldSaveConfig = false;

// DEEP SLEEP INTERVAL
char sleepIntervalStr[6] = "30";
int timeToSleep = 30;

// DISPLAYING INFORMATION
String dayDisp = "";
String dateDisp = "";
String timeDisp = "";
const unsigned char* iconDisp = nullptr;
float TemperatureDisp = 0;
float TemperatureFeelsLikeDisp = 0;
float humidityDisp = 0;
float windDisp = 0;
float aqiPM25Disp = 0;
int batteryDisp = 0;
bool batteryCharging = false;

// UNIX TO READABLE FORMAT
time_t timeUnix = 0;
struct tm* timeInfo;
const char* days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const char* months[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(4, 5, 6, 7));  // CS, DC, RES, BUSY
Preferences preferences;

void configModeCallback(WiFiManager* myWiFiManager);
void saveConfigCallback();
void batteryCheck();
void initializeScreen();
void loadCoordinatesFromFlash();
void connectWiFiManager();
void runIPGeolocation();
void checkCurrentWeather(HTTPClient& http);
void checkAQIWeather(HTTPClient& http);
void formatUnixTime();
void renderVisuals();
void enterDeepSleep();
void displayError();

void setup() {
  Serial.begin(115200);

  initializeScreen();
  batteryCheck();
  loadCoordinatesFromFlash();

  connectWiFiManager();

  if (lat[0] == '\0' || lon[0] == '\0') {
    runIPGeolocation();
    Serial.println(F("Saved IP Geolocation"));
  }

  HTTPClient http;
  checkCurrentWeather(http);
  checkAQIWeather(http);

  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  Serial.println("WiFi: Off");

  formatUnixTime();
  display.firstPage();
  do {
    displayVisual();
  } while (display.nextPage());

  enterDeepSleep();
}

void initializeScreen() {
  display.init(115200, true, 50, false);
  display.setRotation(0);
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.setTextColor(GxEPD_BLACK);
}

void connectWiFiManager() {
  WiFiManager wm;

  WiFiManagerParameter custom_api_key_field("owm_key", "OpenWeatherMap API Key", apiKey, 33);
  WiFiManagerParameter custom_interval_field("refresh_time", "Refresh Interval (minutes)", sleepIntervalStr, 5);

  wm.addParameter(&custom_api_key_field);
  wm.addParameter(&custom_interval_field);
  wm.setSaveConfigCallback(saveConfigCallback);

  wm.setConnectTimeout(15);
  IPAddress local_IP(192, 168, 0, 150); 
  IPAddress gateway(192, 168, 0, 1); 
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8); 
  wm.setSTAStaticIPConfig(local_IP, gateway, subnet, primaryDNS); 
  wm.setAPCallback(configModeCallback);
  wm.setConfigPortalTimeout(180);

  if (!wm.autoConnect("WeatherPaper-Setup")) {
    Serial.println("Failed to connect or portal timed out. Deep sleeping...");
    displayError(1);
    enterDeepSleep();
  }

  if (shouldSaveConfig) {
    strcpy(apiKey, custom_api_key_field.getValue());
    strcpy(sleepIntervalStr, custom_interval_field.getValue());

    timeToSleep = String(sleepIntervalStr).toInt();
    if (timeToSleep <= 0) timeToSleep = 30;

    preferences.begin("weather-space", false);
    preferences.putString("api_key", String(apiKey));
    preferences.putString("sleep_time", String(timeToSleep));
    preferences.end();

    Serial.println("Config saved to flash memory successfully!");
  }

  if (strlen(apiKey) != 32) {  // counts character
    Serial.println("OpenWeatherMap API Key is missing");

    preferences.begin("weather-space", false);
    preferences.clear();
    preferences.end();

    WiFiManager wm;
    wm.resetSettings();

    ESP.restart();
  }
  Serial.println("WiFi connected successfully. Checking weather.");
}
void saveConfigCallback() {
  Serial.println("Should save config triggered!");
  shouldSaveConfig = true;
}
void configModeCallback(WiFiManager* myWiFiManager) {
  preferences.begin("weather-space", false);
  preferences.clear();
  preferences.end();

  Serial.println("Entered Config Mode. Generating Setup QR Code...");

  String apSSID = myWiFiManager->getConfigPortalSSID();
  String qrContent = "WIFI:S:" + apSSID + ";T:nopass;P:;;";

  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, qrContent.c_str());

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(20, 40);
    display.print("Configure WeatherPaper");

    display.setFont(&FreeSans9pt7b);
    display.setCursor(20, 80);
    display.print("Connect or scan QR code with your phone");
    display.setCursor(20, 105);
    display.print("to connect to setup network.");

    display.setCursor(20, 140);
    display.setFont(&FreeSans9pt7b);
    display.print("WiFi name: ");
    display.setFont(&FreeSans9pt7b);
    display.print(apSSID);

    int scale = 4;
    int offsetX = 145;
    int offsetY = 165;

    for (uint8_t y = 0; y < qrcode.size; y++) {
      for (uint8_t x = 0; x < qrcode.size; x++) {
        if (qrcode_getModule(&qrcode, x, y)) {
          display.fillRect(offsetX + (x * scale), offsetY + (y * scale), scale, scale, GxEPD_BLACK);
        }
      }
    }
  } while (display.nextPage());
}

void loadCoordinatesFromFlash() {
  preferences.begin("weather-space", true);
  lat = preferences.getString("latitude", "");
  lon = preferences.getString("longitude", "");

  String savedKey = preferences.getString("api_key", "");
  strcpy(apiKey, savedKey.c_str());

  String savedInterval = preferences.getString("sleep_time", "30");
  strcpy(sleepIntervalStr, savedInterval.c_str());

  timeToSleep = savedInterval.toInt();
  if (timeToSleep <= 0) timeToSleep = 30;


  preferences.end();
  Serial.println("Data loaded from flash: Lat=" + lat + ", Lon=" + lon + ", Key=" + String(apiKey) + ", Refresh=" + String(timeToSleep) + " mins");
}

void saveCoordinatesToFlash(String newLat, String newLon) {
  preferences.begin("weather-space", false);  

  preferences.putString("latitude", newLat);
  preferences.putString("longitude", newLon);

  preferences.end();
  Serial.println("Coordinates permanently saved to Flash Memory!");
}

void runIPGeolocation() {
  HTTPClient http;
  http.begin("http://ip-api.com/json/");
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      float newLat = doc["lat"];
      float newLon = doc["lon"];

      lat = String(newLat, 3);
      lon = String(newLon, 3);

      saveCoordinatesToFlash(lat, lon);
      Serial.println("IP Geolocation parsing successful!");
    } else {
      Serial.println("IP Geolocation JSON parsing failed");
    }
  } else {
    Serial.printf("IP Geolocation HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

// Checking weather
void checkCurrentWeather(HTTPClient& http) {
  String currentWeatherURL = "http://api.openweathermap.org/data/3.0/onecall?lat=" + lat + "&lon=" + lon + "&exclude=minutely,hourly,daily" + "&appid=" + String(apiKey) + "&units=metric";

  http.begin(currentWeatherURL);
  http.addHeader("Connection", "keep-alive");

  int httpCode = http.GET();

  if (httpCode == 200) {
    WiFiClient* stream = http.getStreamPtr();

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, *stream);

    if (!error) {
      if (doc["cod"]) {
        Serial.println("OpenWeatherMap API Key is invalid");

        preferences.begin("weather-space", false);
        preferences.clear();
        preferences.end();

        WiFiManager wm;
        wm.resetSettings();

        ESP.restart();
      }
      TemperatureDisp = doc["current"]["temp"];
      TemperatureFeelsLikeDisp = doc["current"]["feels_like"];
      humidityDisp = doc["current"]["humidity"];
      unsigned long time = doc["current"]["dt"];
      unsigned long timeOffset = doc["timezone_offset"];
      String icon = doc["current"]["weather"][0]["icon"];
      windDisp = doc["current"]["wind_speed"];
      timeUnix = time + timeOffset;

      int iconId = (icon[0] - '0') * 10 + (icon[1] - '0');

      switch (iconId) {
        case 1:
          iconDisp = (icon[2] == 'n') ? epd_bitmap_clear_sky_night : epd_bitmap_clear_sky;
          break;
        case 2: iconDisp = epd_bitmap_few_clouds; break;
        case 3: iconDisp = epd_bitmap_scattered_clouds; break;
        case 4: iconDisp = epd_bitmap_broken_clouds; break;
        case 9: iconDisp = epd_bitmap_shower_rain; break;
        case 10: iconDisp = epd_bitmap_rain; break;
        case 11: iconDisp = epd_bitmap_thunderstorm; break;
        case 13: iconDisp = epd_bitmap_snow; break;
        case 50: iconDisp = epd_bitmap_mist; break;
        default: iconDisp = epd_bitmap_clear_sky; break;
      }

      Serial.println("Data parsing successful: 1/2");
    } else {
      Serial.println("JSON parsing failed: 1/2");
    }
  } else {
    Serial.printf("HTTP GET failed: 1, error: %s\n", http.errorToString(httpCode).c_str());
  }
}
void checkAQIWeather(HTTPClient& http) {
  String aqiWeatherURL = "http://api.openweathermap.org/data/2.5/air_pollution?lat=" + lat + "&lon=" + lon + "&appid=" + String(apiKey);

  http.begin(aqiWeatherURL);
  int httpCode = http.GET();

  if (httpCode == 200) {
    WiFiClient* stream = http.getStreamPtr();

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, *stream);

    if (!error) {
      aqiPM25Disp = doc["list"][0]["components"]["pm2_5"];

      Serial.println("Data parsing successful: 2/2");
    } else {
      Serial.println("JSON parsing failed: 2/2");
    }
  } else {
    Serial.printf("HTTP GET failed: 3, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

// Battery
void batteryCheck() {
  pinMode(batteryChargingPin, INPUT);

  if (digitalRead(batteryChargingPin) == HIGH) {
    batteryCharging = true;
    Serial.println("Battery: Charging");
  } else {
    batteryCharging = false;
    pinMode(batteryAdcPin, INPUT);
    uint32_t Vbatt = 0;
    for (int i = 0; i < 16; i++) {
      Vbatt = Vbatt + analogReadMilliVolts(batteryAdcPin); 
      delayMicroseconds(50); 
    }
    float Vbattf = 2 * Vbatt / 16 / 1000.0;
    Serial.println("Battery voltage: " + String(Vbattf));
    float result = (Vbattf - 3.4) * (99.0 / (4.2 - 3.4));
    result = constrain(result, 0, 99);
    batteryDisp = result;
    if (batteryDisp == 0) {
      displayError(0);
      enterDeepSleep();
    }
  }
}

// Visual rendering
void displayVisual() {
  display.fillScreen(GxEPD_WHITE);

  // DAY AND DATE
  display.setFont(&FreeSansBold18pt7b);
  display.setCursor(27, 45);
  display.print(dayDisp);

  display.setFont(&FreeSans9pt7b);
  display.setCursor(27, 72);
  display.print(dateDisp);

  // WEATHER ICON
  display.drawBitmap(295, 16, iconDisp, 64, 64, GxEPD_BLACK);

  // TEMPERATURE
  display.drawBitmap(30, 94, epd_bitmap_temp_sign, 26, 26, GxEPD_BLACK);
  display.setFont(&FreeSans12pt7b);
  display.setCursor(72, 116);
  if (TemperatureDisp < 10) {
    display.print(TemperatureDisp, 2);
  } else {
    display.print(TemperatureDisp, 1);
  }
  display.drawBitmap(122, 100, epd_bitmap_celcius_sign, 23, 13, GxEPD_BLACK);

  // HUMIDITY
  display.drawBitmap(205, 95, epd_bitmap_humid_sign, 26, 26, GxEPD_BLACK);
  display.setFont(&FreeSans12pt7b);
  display.setCursor(247, 116);
  display.print(humidityDisp, 0);
  display.print("%");

  // COMFORT HEADER
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(53, 169);
  display.print("Comfort");

  // FEELS LIKE TEMPERATURE
  display.drawBitmap(35, 189, epd_bitmap_body_temp, 26, 26, GxEPD_BLACK);
  display.setFont(&FreeSans12pt7b);
  display.setCursor(77, 211);
  if (TemperatureFeelsLikeDisp < 10) {
    display.print(TemperatureFeelsLikeDisp, 2);
  } else {
    display.print(TemperatureFeelsLikeDisp, 1);
  }
  display.drawBitmap(127, 195, epd_bitmap_celcius_sign, 23, 13, GxEPD_BLACK);

  // WIND SPEED
  display.drawBitmap(35, 228, epd_bitmap_wind, 26, 26, GxEPD_BLACK);
  display.setCursor(77, 248);
  display.setFont(&FreeSans12pt7b);
  if (windDisp >= 10) {
    display.print(windDisp, 1);
  } else {
    display.print(windDisp, 2);
  }
  display.setFont(&FreeSans9pt7b);
  display.print(" km/h");

  // HEALTH HEADER
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(263, 169);
  display.print("Health");

  // AQI PM2.5
  display.setFont(&FreeSans9pt7b);
  display.setCursor(225, 204);
  display.print("Air Quality - PM2.5");
  if (aqiPM25Disp < 12) {
    display.drawBitmap(233, 219, epd_bitmap_happy_face, 30, 30, GxEPD_BLACK);
  } else if (aqiPM25Disp < 35.4) {
    display.drawBitmap(233, 219, epd_bitmap_neutral_face, 30, 30, GxEPD_BLACK);
  } else {
    display.drawBitmap(233, 219, epd_bitmap_sad_face, 30, 30, GxEPD_BLACK);
  }
  display.setFont(&FreeSans12pt7b);
  display.setCursor(274, 242);
  if (aqiPM25Disp >= 10) {
    display.print(aqiPM25Disp, 1);
  } else {
    display.print(aqiPM25Disp, 2);
  }
  display.drawBitmap(323, 227, epd_bitmap_aqi_symbol, 46, 17, GxEPD_BLACK);

  // BATTERY PERCENTAGE
  display.drawBitmap(350, 283, epd_bitmap_battery_sign, 30, 16, GxEPD_BLACK);
  display.setFont(&Petme8x8);
  if (batteryCharging == true) {
    display.setCursor(362, 295);
    display.print("+");
  } else {
    display.setCursor(356, 295);
    display.print(batteryDisp);
  }

  // UPDATE TIMESTAMP
  display.setFont();
  display.setCursor(32, 288);
  display.print("Last Updated: " + timeDisp);

  // LINE
  display.drawLine(0, 275, 399, 275, 0);  
  display.drawLine(0, 136, 399, 136, 0); 
  display.drawLine(200, 136, 200, 275, 0); 
}
void formatUnixTime() {
  timeInfo = gmtime(&timeUnix);

  char dayStr[15];
  char dateStr[20];
  char timeStr[15];
  sprintf(dayStr, "%s", days[timeInfo->tm_wday]);
  sprintf(dateStr, "%d %s %d", timeInfo->tm_mday, months[timeInfo->tm_mon], timeInfo->tm_year + 1900);
  sprintf(timeStr, "%02d:%02d", timeInfo->tm_hour, timeInfo->tm_min);

  dateDisp = dateStr;
  dayDisp = dayStr;
  timeDisp = timeStr;
}
void displayError(int type) {
  display.fillScreen(GxEPD_WHITE);
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(20, 40);
  if (type == 0) {
    display.print("Charge Battery!");
  } else if (type == 1) {
    display.print("WiFi Timeout Error");
  }
  display.display();
}

void enterDeepSleep() {
  Serial.println("Going to deep sleep.");
  uint64_t sleepCycles = (uint64_t)timeToSleep * 1000000 * 60;
  esp_sleep_enable_timer_wakeup(sleepCycles);
  esp_deep_sleep_start();
}

void loop() {
}