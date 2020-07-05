/***************************************************************************
  Hacked by Robert L. Read, 2020
  Networking code by Geoff Mulligan 2020
  designed to support ethernet using esp wifi chip

  All Adafruit libraries written by Limor Fried for Adafruit Industries.

  Modififications released under BSD to avoid complication (I prefer GPL).
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>

#include <Ethernet.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <EthernetUdp.h>
#include <Dns.h>

#include <PIRDS.h>
#include <SFM3X00.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <Adafruit_ADS1015.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <display.h>
#include <pressure.h>
#include <networking.h>
#include <data_tx.h>
#include <debug_serial.h>
#include <flow.h>
#include <oxygen.h>
#include <utils.h>


void setup() {
  Serial.begin(115200);
  Wire.begin();
  while (!Serial); // Wait for serial connection to be ready

  // DISPLAY
  display.display_init();
  
  // SENSORS
  flow.init();

  pressure.init();

  oxygen.init();
  
  // NETWORKING
  networking.init();
}

void loop() {
  // Check the clock has ticked correctly.
  unsigned long m = millis();
  if (m > sample_tick_millis) {
    sample_tick_millis = m;
  } else {
    // Something went wrong - warn the user.
    Serial.println(F("ERROR! loop(): unticked clock cycle.")); // Todo: improve error handling
    return;
  }

  // Time since boot in milliseconds - used in the calculations below.
  unsigned long sample_ms = millis();

  pressure.read_pressure_sensors(sample_ms);
  oxygen.read_oxygen_sensor(sample_ms);
  flow.read_flow_sensor(sample_ms);
  debug_serial.debug_clear();

  display.update_display();
}
