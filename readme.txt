WEATHER STATION MkII
(c) 2020 by Boris Emchenko

Необходимые библиотеки:


// Стандартные из ядра:
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

//Для работы с OTA
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>


(!)WiFiManager.h - модифицированная библиотека
	оригинал: https://github.com/tzapu/WiFiManager
	* @author Creator tzapu
	* @author tablatronix
	* @version 0.0.0
	* @license MIT

	* based on 2.0.3-alpha
	* v 2.0.3-alpha_0.3
	* Modified by BorisE 
 
	 * 2.0.3-alpha_0.3 [2020/08/06]
	 *                  - init overload with int default value
	 * 2.0.3-alpha_0.2 [2020/08/06]
	 *                  - timeout constants set in header
	 * 2.0.3-alpha_0.1 [2020/08/05]
	 *                  - go to root (through javascript) after executing Exit (WiFiManager::handleExit()), Reboot (WiFiManager::handleReset()), WiFiManager::handleParamSave(), WiFiManager::handleWifiSave()
	 *                  - WiFiManagerParameter.init method made public

(!)BME280_I2C.h - модифицированная библиотека
	Small optimization of  cactus.io library by Boris
	version 1.02

	Changelog (by Boris)
	1.02   [19.07.2020]    added getPressure_atm() and getPressure_mmHg() for getting pressure in standart atmoshere/mm Hg respectively
							changed getPressure_HP to getPressure_P because of error in naming (pascals not hectopascals)
	1.01   [18.07.2020]    added SDA and SCK pins configuration to begin method
 
	7/1/18 Bug Fix by Wilhelm  Fixed getTemperature_C and getTemperature_F

(!)MLX90614.h - модифицированная библиотека
	This is a library for the MLX90614 Temp Sensor
	with small optimization from Boris

	Designed specifically to work with the MLX90614 sensors in the
	adafruit shop
	----> https://www.adafruit.com/products/1748
	----> https://www.adafruit.com/products/1749

	Written by Limor Fried/Ladyada for Adafruied in any redistribution

	version 1.01

	Changelog (by Boris)
	1.01   [18.07.2020]    added SDA and SCK pins configuration to begin method
 


OneWire.h - стандартая библиотека
	version=2.3.5
	author=Jim Studt, Tom Pollard, Robin James, Glenn Trewitt, Jason Dangel, Guillermo Lovato, Paul Stoffregen, Scott Roberts, Bertrik Sikken, Mark Tillotson, Ken Butcher, Roger Clark, Love Nystrom
	maintainer=Paul Stoffregen
	url=http://www.pjrc.com/teensy/td_libs_OneWire.html
	https://github.com/PaulStoffregen/OneWire
 

Wire.h - стандартая версия из пакета
    version=1.0
    author=Arduino
    maintainer=Ivan Grokhotkov <ivan@esp8266.com>
    sentence=Allows the communication between devices or sensors connected via Two Wire Interface Bus. For esp8266 boards.  
    AppData\\Local\\Arduino15\\packages\\esp8266\\hardware\\esp8266\\2.7.4\\libraries\\Wire

FS.h - стандартная библиотека для работы с SPIFFS
    FS.cpp - file system wrapper
    Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
    This file is part of the esp8266 core for Arduino environment. 
 
    
Ticker.h - стандартая библиотека

DallasTemperature.h - стандартая библиотека
	VERSION 3.7.2 BETA DATE: 6 DEC  2011
	- Jordan Hochenbaum [jhochenbaum@gmail.com] updated library for compatibility with Arduino 1.0. 
	
	You can find the latest version of the library at http://milesburton.com/index.php?title=Dallas_Temperature_Control_Library 
	The OneWire code has been derived from 	http://www.arduino.cc/playground/Learning/OneWire.
	Miles Burton <miles@mnetcs.com> originally developed this library.
	Tim Newsome <nuisance@casualhacker.net> added support for multiple sensors on the same bus.
	Guil Barros [gfbarros@bappos.com] added getTempByAddress (v3.5)
	Rob Tillaart [rob.tillaart@gmail.com] added async modus (v3.7.0) 

BH1750.h - стандартая библиотека
	version=1.1.4
	author=Christopher Laws
	maintainer=Christopher Laws
	url=https://github.com/claws/BH1750
	[![Build Status](https://travis-ci.org/claws/BH1750.svg?branch=master)](https://travis-ci.org/claws/BH1750)

DHTesp.h - стандартая библиотека
	DHT sensor library for ESPx
	version=1.17
	author=beegee_tokyo
	maintainer=beegee_tokyo <beegee@giesecke.tk>
	url=http://desire.giesecke.tk/index.php/2018/01/30/esp32-dht11/
	An Arduino library for reading the DHT family of temperature and humidity sensors.    
	Forked from [arduino-DHT](https://github.com/markruys/arduino-DHT)     
	Original written by Mark Ruys, <mark@paracas.nl>.    

	Changes to the original library:
	--------
	- 2017-12-12: Renamed DHT class to DHTesp and filenames from dht.* to DHTesp.* to avoid conflicts with other libraries - beegee-tokyo, <beegee@giesecke.tk>.    

	
ArduinoJson.h - загруженная библиотека
	// https://github.com/bblanchon/ArduinoJson
	name=ArduinoJson
	version=6.16.1
	author=Benoit Blanchon <blog.benoitblanchon.fr>
	maintainer=Benoit Blanchon <blog.benoitblanchon.fr>
	repository=https://github.com/bblanchon/ArduinoJson.git

****************************************************************************************************************************************

Hardware:
    Wemos D1 R1
    Настройки компиляции: 4MB (FS 2MB, OTA 1 MB)
    