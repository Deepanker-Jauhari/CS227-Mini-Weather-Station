#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>
#include <avr/dtostrf.h> 
#include <ss_oled.h> 
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>


char auth[] = "wQpvBA1EyKYuHJ4FPLinJQOVy9K-xURA";
char ssid[] = "DEEJAY-PREDATOR";
char pass[] = "26122003";

Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

#define SDA_PIN 5
#define SCL_PIN 4
#define RESET_PIN -1
#define OLED_ADDR -1
#define FLIP180 0
#define INVERT 0
#define USE_HW_I2C 0
#define MY_OLED OLED_128x64
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

SSOLED ssoled;
BlynkTimer timer; 


void setup() {
  if (!bme.begin()) { 
    while (1) delay(10);
    Serial.println(F("Could not find a valid BME280 sensor, check wiring or "
                      "try a different address!"));
  }

  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();

  oledInit(&ssoled, MY_OLED, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, SDA_PIN, SCL_PIN, RESET_PIN, 400000L);
  oledFill(&ssoled, 0x0, 1);
  
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, myupdate);  // runs every 5 seconds
  myupdate(); 
}

void loop() {
   Blynk.run();
   timer.run(); 
}

void myupdate(){
  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);

  float ctemp = temp_event.temperature;  // get temp in centigrade
  float ftemp = 32 + (9*ctemp)/5;  // convert temp to fahrenheit
  float hum = humidity_event.relative_humidity;  // get relative humidity
  float mpress = pressure_event.pressure; // get pressure in mm

  char mytemp[8]; 
  dtostrf(ftemp, 6, 2, mytemp); // convert temp to a string
  char myhum[8]; 
  dtostrf(hum, 6, 2, myhum); // convert humidity to a string
  char mypress[8]; 
  dtostrf(mpress, 6, 2, mypress); // convert pressure to a string

  // update OLED display
  oledWriteString(&ssoled, 0,2,1,(char *)"Weather Station", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0,2,3,(char *)"Temp = ", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0,50,3,(char *)mytemp, FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0,105,3,(char *)"F", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0,2,5,(char *)"Humid=", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0,50,5,(char *)myhum, FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0,105,5,(char *)"%", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0,2,7,(char *)"Press=", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0,50,7,(char *)mypress, FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0,105,7,(char *)"in", FONT_NORMAL, 0, 1);

  // push new weather data to Blynk
  Blynk.virtualWrite(V3, ftemp);
  Blynk.virtualWrite(V4, hum); 
  Blynk.virtualWrite(V5, mpress);  
}
  