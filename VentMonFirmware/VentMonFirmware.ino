/***************************************************************************
  Hacked by Robert L. Read, 2020
  Networking code by Geoff Mulligan 2020
  designed to support ethernet using esp wifi chip

  All Adafruit libraries written by Limor Fried for Adafruit Industries.

  Modififications released under BSD to avoid complication (I prefer GPL).
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <PIRDS.h>

// VentMon Includes
#include "config.h"
#include "display.h"
#include "networking.h"
#include "data_tx.h"
#include "pressure.h"
#include "flow.h"
#include "oxygen.h"
#include "debug_serial.h"

unsigned long sample_tick_millis;

// Program entry point
void setup() {
  
  Serial.begin(115200);
  while (!Serial); // Wait for serial connection to be ready

  Wire.begin();
  
  // DISPLAY
  display_init();
  
  // SENSORS
  flow_init();
  pressure_init();
  oxygen_sensor_init();
  
  // NETWORKING
  networking_init();
}

// VentMon uses a simple loop to read sensors and
// output the data in a JSON PIRDS format.
void loop() {
  check_tick();

  unsigned long sample_ms = millis();

  read_pressure_sensors(sample_ms);
  read_oxygen_sensor(sample_ms);
  read_flow_sensor(sample_ms);
  
  Serial.flush();
  update_display();
}

// Check the loop has advanced correctly.
void check_tick(){
  unsigned long m = millis();
  if (m > sample_tick_millis) {
    sample_tick_millis = m;
  } else {
    // Something went wrong - warn the user.
    Serial.println(F("ERROR! loop(): unticked clock cycle.")); // Todo: improve error handling
	
    return;
  }
}
