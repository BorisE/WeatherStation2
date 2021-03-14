/*
  WEATHER STATION MkII
  (c) 2020 by Boris Emchenko

  TODO:
  - NTP server
  - data logging using SPIFFS
  - Capacitive rain sensor
  - UV sensor?
  - CO2 sensor?
  - OTA web update
  - Deepsleep mode?

 Changes:
   ver 2.8a 2021/03/14 [455860/33060]
                      - sending rain sensor to Narodmon
                      - additional checking for CIDX before sending to Narodmon
   ver 2.8 2021/03/14 [455812/33060]
                      - sending rain sensor to Narodmon
   ver 2.7b 2021/02/10 [455756/33044]
                      - more solid data checking before sending to NARODMON
   ver 2.7a 2021/02/07 [455644/33044]
                      - bugfix
   ver 2.7 2021/01/20 [455644/33044]
                      - checking validity before sending to NARODMON
   ver 2.6 2020/11/28 [455656/33040]
                      - moved config to Configuration.h
                      - narodmon data backup sending: temp/dht1/dht2/bme
   ver 2.5 2020/11/02 [455544/33040]
                      - additional DHT22 sensor
   ver 2.4с 2020/11/02 [454744/32904]
                      - moving to narodmon GET sending failed and left POST with some modifications
   ver 2.4 2020/11/02 [454480/32884]
                      - moving to narodmon GET sending
   ver 2.3 2020/11/01 [454156/32808]
                      - rain sensor
                      - dht verbose data corrected
   ver 2.2 2020/08/28 [453716/32776]
                      - send data to narodmon.ru
   ver 2.1c 2020/08/28 [452668/32608]
                      - moved to DHTesp lib (not finally, just to test)
                      - some bugs in html cleared
   ver 2.02 2020/08/23 [446280/32388]
                      - html design update2 (astrodata through suncalc.js lib)
   ver 2.01 2020/08/23 [446280/32388]
                      - html design update (header, currtime, sunrise/sunset through sun.js lib)
   ver 2.0 2020/08/22 [446280/32388]
                      - webserver: load webpages from SPIFFS
                      - new card design
   ver 1.2 2020/08/22 [444088/32260]
                      - ArduinoOTA update
   ver 1.11 2020/08/15 [430436/31744]
                      - memory (F(..)) optimization
   ver 1.1 2020/08/09 [430424/32244]
                      - added BH1750FVI (lux) sensor
                      - prepared (but not moved yet!) new HTML templates
   ver 1.02 2020/08/07 [427520/32200]
                      - ds18b20 connection tests to pass out of scale values
                      - config file changed to config.cfg
   ver 1.01 2020/08/06 [427400/32176]
                      - added SDA,SCL,DHT22 pin to config
                      - needs custom WiFiManager v2.0.3-alpha_0.3
   ver 1.00 2020/08/06 [426688/32000]
                      - store settings in special structure
                      - convert config to JSON format
                      - load / store json config on SPIFFS
                      - restart ESP on config chage (to take hardware settings into account)
   ver 1.00a2 2020/08/06 [385944/31656]
                      - blinking led (using ticker.h) during config mode
                      - some optimization
   ver 1.00a1 2020/08/05 [385764/31596]
                      - config portal is working!
                      - custom pararmeters to wificonfig (POST_URL and OneWirePin)
                      - WiFiManager lib customized (on 2.0.3-alpha_0.1): back redirect, WiFiManagerParameter->init method made public
   ver 0.92 2020/08/03 [383448/31388] 
                      - more WiFi connectivity optimization
   ver 0.91 2020/08/03 [383416/31388] 
                      - Solve issue with entering ConfigPortal in the middle on running
                      - Optimizing WiFi connectivity checking
                      - Webpages correction
   ver 0.9 2020/08/02 [383396/31808] 
                      - Enter config mode from web interface (strange behaviour) 
                      - Some HTML correction
   ver 0.8 2020/08/02 [381360/31752] 
                      - Regular reconnect attempts if WiFi credentials was specified earlier (useful after house power loss startup - WiFi Router need can't load before ESP boot up)
                      - WiFi regular check in loop
   ver 0.7 2020/08/02 [381012/31212] 
                      - WiFiManager lib to configure WiFi connection
   ver 0.6 2020/08/01 [333456/28808] 
                      - moving to DallasTemperature lib because local hangs sensor
   ver 0.5 2020/07/19 [331924/28788] 
                      - json post
                      - bugfixes
   ver 0.4 2020/07/19 - MLX implemented
                      - return json data
                      - webpage js update 
   ver 0.3 2020/07/19 - onewire implemented
                      - webpages templates changed
   ver 0.2 2020/07/19 - i2c and bme280 implemented
   ver 0.1 2020/07/18 - web data sending implemented (as prototype)
                      - web server implemented 
                      - DHT22 implemented
*/

//Compile version
#define VERSION "2.8a2"
#define VERSION_DATE "20210314"

#include <FS.h>          // this needs to be first, or it all crashes and burns...
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>

#include <Wire.h>
#include <BME280_I2C.h>
#include <OneWire.h>
#include <MLX90614.h>
#include <DallasTemperature.h>
#include <BH1750.h>
#include "DHTesp.h"

#include <Ticker.h>//for LED status

#include "Configuration.h"

ConfigStruct configData;                              // <- global configuration object

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

unsigned long _last_HTTP_SEND=0;

#define NONVALID_TEMPERATURE_MIN -55
#define NONVALID_TEMPERATURE_MAX 100
#define NONVALID_CIDX_MIN -5
#define NONVALID_CIDX_MAX 50
#define NONVALID_PRESSURE 0
#define NONVALID_HUMIDITY 0
#define NONVALID_LUX -100

Ticker ticker;
const int STATUS_LED = LED_BUILTIN;


enum {DHT22_SAMPLE, DHT_TEMPERATURE, DHT_HUMIDITY, DHT_DATAPTR};  // DHT functions enumerated
enum {DHT_OK = 0, DHT_ERROR_TIMEOUT = -1, DHT_ERROR_CRC = -2, DHT_ERROR_UNKNOWN = -3};  // DHT error codes enumerated
float dhtTemp = NONVALID_TEMPERATURE_MIN;
float dhtHum =0;
unsigned long _lastReadTime_DHT=0;
DHTesp dht;
float dhtTemp2 = NONVALID_TEMPERATURE_MIN;
float dhtHum2 =0;
DHTesp dht2;

// Create BME280 object
BME280_I2C bme(MY_BME280_ADDRESS);
float bmePres = NONVALID_PRESSURE;
float bmeTemp = NONVALID_TEMPERATURE_MIN;
float bmeHum  = NONVALID_HUMIDITY;
unsigned long _lastReadTime_BME=0;

OneWire  OneWireBus;  
float OW_Temp1=NONVALID_TEMPERATURE_MIN;
unsigned long _lastReadTime_OW=0;

DallasTemperature ds18b20(&OneWireBus);

// MLX90614 part
MLX90614 mlx = MLX90614();
float mlxAmb = NONVALID_TEMPERATURE_MIN;
float mlxObj = NONVALID_TEMPERATURE_MIN;
unsigned long _lastReadTime_MLX=0;

//BH1750FVI light sensor part
BH1750 lightMeter (BH1750_ADDR);
float bh1750Lux = NONVALID_LUX;
unsigned long _lastReadTime_BH1750=0;

//Rain sensor
unsigned int rainSensor;

//Send to NAROMDMON
bool bNarodmon_Send_Data=NARODMON_DATA_SENDING_DEFAULT;
unsigned long _last_NARODMON_SEND;

unsigned long currenttime;              // millis from script start 

bool bOutput=false;

/********************************************************
*     SETUP
 *******************************************************/
void setup(void) {
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  Serial.begin(115200);
  //Serial.setDebugOutput(true);  
  
  // Wait for serial to initialize.
  while(!Serial) { }
  Serial.println("");

  // Greeting message
  Serial.println(F("WEATHER STATION MkII"));
  Serial.print ("v");
  Serial.print (VERSION);
  Serial.print (" [");
  Serial.print (VERSION_DATE);
  Serial.println ("]");

  //Load config data
  LoadDefaults();
  LoadConfigData();

  ////////////////////////////////
  // WiFI managent part
  WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);
  //WiFi_init();
  //wm.resetSettings(); //reset settings - wipe credentials for testing
  //stop until connection wouldn't be established
  //while (WiFi.status() != WL_CONNECTED) <- don't block after timeout. Measure and then try again (in loop cycle)
  {
    WiFi_CheckConnection(WIFI_CONFIG_PORTAL_WAITTIME_STARTUP);
  }

  ////////////////////////////////
  // MDNS INIT
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    Serial.print(F("Open http://"));
    Serial.print(host);
    Serial.println(F(".local to access WeatherStation interface"));
  }

  ////////////////////////////////
  // WebServer INIT
  server.on("/table", handleRoot);
  server.on("/json", handleJSON);
  server.on("/configmode", handleConfigMode);
  server.on("/ping", handlePingRequest);
  server.onNotFound(handleSPIFSS);

  httpUpdater.setup(&server, "/update");

  server.begin();
  Serial.println(F("HTTP server started"));

  ////////////////////////////////
  // OTA Update
  ArduinoOTA.setHostname(host);
  ArduinoOTA.setPort(OTA_PORT);
  //ArduinoOTA.setPassword("esp8266");
  ArduinoOTA.onStart(onStartOTA);
  ArduinoOTA.onEnd([]() {
    Serial.println("\n[OTA] End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError(onErrorOTA);
  ArduinoOTA.begin();
  Serial.println("[OTA] OTA ready");

  
  ////////////////////////////////
  // START HARDWARE
  Wire.begin(configData.I2CSDAPin, configData.I2CSCLPin);

  dht.setup(configData.DHT22Pin, DHTesp::DHT22);
  dht2.setup(DHT_2_PIN_DEFAULT, DHTesp::DHT22);
   
  //init BME280 sensor
  if (!bme.begin(configData.I2CSDAPin, configData.I2CSCLPin)) 
  {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
  } 

  //MLX
  mlx.begin(configData.I2CSDAPin, configData.I2CSCLPin);  

  //Dallas Sensors
  OneWireBus.begin(configData.OneWirePin);
  ds18b20.begin();
  Serial.print(F("OneWire devices: "));
  Serial.println(ds18b20.getDeviceCount());
  ds18b20.getAddress(OW_Temp1Addr, 0); //try to read address for device 0

  //BH1750
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
}

/********************************************************
*     LOOP
 *******************************************************/
void loop(void) {
  bOutput=false;
  currenttime = millis();
  
  server.handleClient();
  MDNS.update();
  ArduinoOTA.handle();
  
  // Send data to webserver
  // Every given interval
  if ( currenttime - _last_HTTP_SEND > POST_DATA_INTERVAL ) {
      /* try to send data. test ret status. 
       * if no connection start CheckConnection procedure  */
      if (!HTTP_sendJSON())
      {
        server.stop(); // stop web server because of conflict with WiFi manager
        WiFi_CheckConnection(WIFI_CONFIG_PORTAL_WAITTIME);
        server.begin(); // start again
      }
      _last_HTTP_SEND = currenttime;
  }
  if ( bNarodmon_Send_Data && currenttime - _last_NARODMON_SEND > POST_NARODMONDATA_INTERVAL ) {
      /* try to send data. test ret status. 
       * if no connection start CheckConnection procedure  */
      if (!NarodMon_sent_POST())
      {
        server.stop(); // stop web server because of conflict with WiFi manager
        WiFi_CheckConnection(WIFI_CONFIG_PORTAL_WAITTIME);
        server.begin(); // start again
      }
      _last_NARODMON_SEND = currenttime;
  }
  

  //DHT Read very time consuming
  //So read only in given interval
  if (_lastReadTime_DHT ==0 || (currenttime - _lastReadTime_DHT) > DHT_READ_INTERVAL)
  {
    bOutput=true;
    //readDHTSensor(dhtTemp, dhtHum);
    printDHT(dhtTemp, dhtHum);
    printDHT_2(dhtTemp2, dhtHum2);
    _lastReadTime_DHT= currenttime;
  }
  
  if (_lastReadTime_BME ==0 || (currenttime - _lastReadTime_BME) > BME_READ_INTERVAL)
  {
    bOutput=true;
    getBMEvalues(bmePres, bmeTemp, bmeHum);
    _lastReadTime_BME= currenttime;
  }

  if (_lastReadTime_OW ==0 || (currenttime - _lastReadTime_OW) > OW_READ_INTERVAL)
  {
    bOutput=true;
    //OW_Temp1 = getOneWireTemp(OW_Temp1Addr);
    ds18b20.requestTemperatures();
    if (ds18b20.isConnected(OW_Temp1Addr))
    {
      OW_Temp1 = ds18b20.getTempC(OW_Temp1Addr);
    }else{
      OW_Temp1 = NONVALID_TEMPERATURE_MIN;
    }
    
    Serial.print("[!OW1:");
    Serial.print(OW_Temp1);
    Serial.println("]");


    rainSensor = 1024 - analogRead(A0);
    Serial.print("[!Rain:");
    Serial.print(rainSensor);
    Serial.println("]");

    _lastReadTime_OW= currenttime;
  }

  if (_lastReadTime_MLX ==0 || (currenttime - _lastReadTime_MLX) > MLX_READ_INTERVAL)
  {
    bOutput=true;
    ReadMLXvalues(mlxAmb, mlxObj);

    _lastReadTime_MLX = currenttime;
  }

  if (_lastReadTime_BH1750 ==0 || (currenttime - _lastReadTime_BH1750) > BH1750_READ_INTERVAL)
  {
    bOutput=true;
    bh1750Lux  = measureLight();

    _lastReadTime_BH1750 = currenttime;
  }



  /*if (bOutput)
  {
    Serial.println(F("[!END]\r\n"));
  }
  */
  //delay(5000);
}
