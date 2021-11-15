#include <WiFiCredentials.h>

/* for Wemos D1 R1
 *  
#define PIN_WIRE_SDA (4)  D14   (by default, but I redefined, see below)
#define PIN_WIRE_SCL (5)  D15   (by default, but I redefined, see below)

//new ESP-12E GPIO2
#define LED_BUILTIN 2       D9

static const uint8_t D0   = 3;
static const uint8_t D1   = 1;
static const uint8_t D2   = 16;
static const uint8_t D3   = 5;                --> SCL
static const uint8_t D4   = 4;                --> SDA
static const uint8_t D5   = 14;               --> DHT_1
static const uint8_t D6   = 12;               --> OneWire bus
static const uint8_t D7   = 13;               --> DHT_2    
static const uint8_t D8   = 0;  /startup pin.  pulled up to Vcc. Don't use as intput. Special care as output
static const uint8_t D9   = 2;  /startup pin. LED.  pulled up to Vcc. Don't use as intput. Special care as output         -->Used as LED
static const uint8_t D10  = 15; /startup pin. pulled down to GND. Don't use as intput. Special care as output
static const uint8_t D11  = 13; /=D7          
static const uint8_t D12  = 12; /=D6
static const uint8_t D13  = 14; /=D5
static const uint8_t D14  = 4;  /=D4
static const uint8_t D15  = 5;  /=D3

GPIO6-GPIO11 - flash pins

*/

/* NODEMCU
#define PIN_WIRE_SDA (4)    D2
#define PIN_WIRE_SCL (5)    D1

#define LED_BUILTIN 2       D4
#define LED_BUILTIN_AUX 16  D0

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;
*/

/* Wemos D1 Mini
#define PIN_WIRE_SDA (4)  D2
#define PIN_WIRE_SCL (5)  D1

#define LED_BUILTIN 2

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t RX   = 3;
static const uint8_t TX   = 1;
 */

//I2C wire
#define SDA_PIN_DEFAULT D4
#define SCL_PIN_DEFAULT D3

//DHT pin
#define DHT_PIN_DEFAULT D5
#define DHT_2_PIN_DEFAULT D7

//BME280
#define MY_BME280_ADDRESS (0x76)
#define SEALEVELPRESSURE_HPA (1013.25)

//OneWire
#define ONE_WIRE_BUS_PIN_DEFAULT D6 // Data wire is plugged into this port 
//ROM = 28 6D A3 68 4 0 0 F8
uint8_t OW_Temp1Addr[8] = { 0x28, 0x6D, 0xA3, 0x68, 0x4, 0x0, 0x0, 0xF8 };

//BH1750FVI light sensor part
#define BH1750_ADDR 0x23        // if ADD pin to GROUND, 0x23; if to VCC - 0x5C

//Rain sensor
#define RAIN_PIN_DEFAULT A0 // Default pin for Resistance Rain Sensor. The only one for ESP8266 :)


//Send to NAROMDMON
#define NARODMON_DATA_SENDING_DEFAULT true         // Default value for narodmon sending flag
#define NARODMON_SERVER_POST  "http://narodmon.ru/post"
#define NARODMON_SERVER_GET   "http://narodmon.ru/get"


#define DEFAULT_POST_URL "http://192.168.0.200/weather/adddata.php"

/*************************************************************
*	CALCULATIONS INTERVAL SETTINGS
*************************************************************/
#define POST_DATA_INTERVAL          120000    // 120000 = 2 мин
#define POST_NARODMONDATA_INTERVAL  320000    //320000 = 5.33 мин
#define JS_UPDATEDATA_INTERVAL       10000
#define OW_READ_INTERVAL              5555
#define BH1750_READ_INTERVAL          6666
#define MLX_READ_INTERVAL             7777
#define BME_READ_INTERVAL             9999
#define DHT_READ_INTERVAL            15111


/*************************************************************
  Data validation constants
*************************************************************/
#define NONVALID_TEMPERATURE_MIN -55
#define NONVALID_TEMPERATURE_MAX 100
#define NONVALID_CIDX_MIN -5
#define NONVALID_CIDX_MAX 50
#define NONVALID_PRESSURE_MIN 400
#define NONVALID_PRESSURE_MAX 900
#define NONVALID_LUX_MIN -1
#define NONVALID_LUX_MAX 1000000
#define NONVALID_HUMIDITY_MIN 0
#define NONVALID_HUMIDITY_MAX 101
#define NONVALID_RAINSENSOR_MIN 0
#define NONVALID_RAINSENSOR_MAX 1025


/*************************************************************
	WiFi Config portal
*************************************************************/
#define WIFI_CONFIG_PORTAL_WAITTIME  30
#define WIFI_CONFIG_PORTAL_WAITTIME_STARTUP  60

const char* ssid = "WeatherStation";
const char* host = "weather";

/*************************************************************
	OTA
*************************************************************/
#define OTA_PORT 18266

const char *configFilename = "/config.cfg";  


/*************************************************************
	DECLARTION
*************************************************************/
struct ConfigStruct {
  char POST_URL[101];
  uint8_t OneWirePin;
  uint8_t I2CSDAPin;
  uint8_t I2CSCLPin;
  uint8_t DHT22Pin;
  uint8_t RainPin;
};
