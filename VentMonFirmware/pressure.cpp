// Adafruit BME680 - Temperature, Humidity, Pressure and Gas Sensor
// Product page: http://www.adafruit.com/products/3660
// Source code: https://github.com/adafruit/Adafruit_BME680
// API docs: https://adafruit.github.io/Adafruit_BME680/html/class_adafruit___b_m_e680.html

#include <PIRDS.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include "data_tx.h"

#define DEBUG_LOG true
#define SEALEVELPRESSURE_HPA (1013.25)
#define SENSOR_SENTINEL -999

Adafruit_BME680 sensor[2]; // I2C

// TODO:
// It would be nice to support more than one,
// but the BME680 only has two addresses (0x77, and 0x76 when SD0 tied to GND).

// BME680 sensor addresses.
// !!! Unless physical hardware changes the ambient sensor should have an address of
// 0x76 while the airway sensor should have an address of 0x77 on the I2C bus.
// Do not change this unless directed to do so. !!!
#define AMBIENT_SENSOR_ADDRESS  0x76
#define AIRWAY_SENSOR_ADDRESS   0x77

// These values should match the order the sensors occur in the array addr (below)
#define AIRWAY_PRESSURE_SENSOR  0
#define AMBIENT_PRESSURE_SENSOR 1

// BME680 sensor status flag.
bool sensor_connected[2] = {false, false};
uint8_t sensor_addr[2] = {AIRWAY_SENSOR_ADDRESS, AMBIENT_SENSOR_ADDRESS};

signed long pressure_max_display = 0;
signed long pressure_min_display = 0;
signed long display_airway_pressure = 0;
signed long display_debug_value = 0;

// Only need to sample the ambient air occasionally
// (say once a minute) for PEEP analysis
int ambient_counter = 0;
//sunsigned long sample_tick_millis = 0;

// We could send out only raw data, and let more powerful computers process things.
// But we have a powerful micro controller here, so we will try to be useful!
// Instead of outputting only the absolute pressure, we will output the differential
// pressure in the Airway. We will compute a differential pressure against the
// ambient air. We will name this D0. We need a moving window to make sure there
// is not jitter.
int amb_wc = 0;
#define AMB_WINDOW_SIZE 4
#define AMB_SAMPLES_PER_WINDOW_ELEMENT 200
// sea level starting pressure.
signed long ambient_window[AMB_WINDOW_SIZE];


// Check if BME pressure sensors were found
// Otherwise try find them!



// Initialize pressure sensor
void init_sensor(int i) {
  uint8_t loc_addr = sensor_addr[i];
  // I don't understand why this API does not work, it seemed to work in the previous version....
  sensor_connected[i] = sensor[i].begin(loc_addr, true);
  //  sensor_connected[i] = sensor[i].begin(sensor_addr[i]);

  if (!sensor_connected[i]) {
	#ifdef DEBUG_LOG
//      debug_print(F("WARNING! BME680 sensor could not be found!\nCheck connection for: "));
  //    debug_print(loc_addr, HEX);
	#endif
  } else {
    // Set up oversampling and filter initialization
    if (i == 0) {
      // sensor[i].setTemperatureOversampling(BME680_OS_8X);
      sensor[i].setTemperatureOversampling(BME680_OS_1X);
      sensor[i].setHumidityOversampling(BME680_OS_1X);
      sensor[i].setPressureOversampling(BME680_OS_1X);
      // sensor[i].setIIRFilterSize(BME680_FILTER_SIZE_3);
      // sensor[i].setGasHeater(320, 150); // 320*C for 150 ms
      sensor[i].setGasHeater(0, 0); // 320*C for 150 ms
    } else if (i == 1) {
      // sensor[i].setTemperatureOversampling(BME680_OS_8X);
      sensor[i].setTemperatureOversampling(BME680_OS_1X);
      sensor[i].setHumidityOversampling(BME680_OS_1X);
      sensor[i].setPressureOversampling(BME680_OS_1X);
      //    sensor[i].setIIRFilterSize(BME680_FILTER_SIZE_3);
      //     sensor[i].setGasHeater(320, 150); // 320*C for 150 ms
      // I believe this feature is not needed or useful for this application
      sensor[i].setGasHeater(0, 0); // 320*C for 150 ms
    }
  }
}

void init_ambient(signed long v) {
  for (int i = 0; i < AMB_WINDOW_SIZE; i++) ambient_window[i] = v;
}



void sensor_check_status() {
  if (!sensor_connected[AIRWAY_PRESSURE_SENSOR]) {
    init_sensor(AIRWAY_PRESSURE_SENSOR);
  }

  if (!sensor_connected[AMBIENT_PRESSURE_SENSOR]) {
    init_sensor(AMBIENT_PRESSURE_SENSOR);
  }
}


// the parameter i here numbers our pressure sensors.
// The sensors may be better, for as per the PIRDS we are returning integer 10ths of a mm of H2O
signed long read_pressure_only(int i)
{
  if (i < 2) {
    if (! sensor[i].performReading()) {
	  #ifdef DEBUG_LOG
//        debug_print(F("WARNING! Failed to read BME sensor:"));
  //      debug_print(sensor_addr[i], HEX);
	  #endif
      sensor_connected[i] = false;
      return -999.0;
    } else {
      // returning resorts in kPa
      // the sensor apparently gives us Pascals...

      return (signed long) (0.5 + (sensor[i].pressure / (98.0665 / 10)));
    }
  } else {
    // internal error! Ideally would publish an internal error event
  }
}

void sensor_report_full(int i)
{
  unsigned long ms = millis();

  if (! sensor[i].performReading()) {
  #ifdef DEBUG_LOG
      Serial.print(F("ERROR! Failed to read BME sensor: "));
      Serial.println(sensor_addr[i], HEX);
  #endif
    sensor_connected[i] = false;
    return;
  }

  char loc = (i == 0) ? 'A' : 'B';
  output_measurement('M', 'T', loc, 0, ms, (signed long) (0.5 + (sensor[i].temperature * 100)));
  output_measurement('M', 'P', loc, 0, ms, (signed long) (0.5 + (sensor[i].pressure / (98.0665 / 10))));
  output_measurement('M', 'H', loc, 0, ms, (signed long) (0.5 + (sensor[i].humidity * 100)));
  output_measurement('M', 'G', loc, 0, ms, (signed long) (0.5 + sensor[i].gas_resistance));
  output_measurement('M', 'A', loc, 0, ms, (signed long) (0.5 + sensor[i].readAltitude(SEALEVELPRESSURE_HPA)));
}

void read_pressure_sensors(uint32_t sample_ms) {

  // Check for BME sensors
  sensor_check_status();

  // We need to use a better sentinel...this is a legal value!
  // units for pressure are cm H2O * 100 (integer 10ths of mm)
  signed long ambient_pressure = SENSOR_SENTINEL;
  signed long pressure_internal = SENSOR_SENTINEL;  // Inspiratory Pathway pressure

  // Check airway pressure
  if (sensor_connected[AIRWAY_PRESSURE_SENSOR]) {
    pressure_internal = read_pressure_only(AIRWAY_PRESSURE_SENSOR);
  }

  // Read ambient pressure
  if (((ambient_counter % AMB_SAMPLES_PER_WINDOW_ELEMENT) == 0) && sensor_connected[AMBIENT_PRESSURE_SENSOR]) {
    ambient_pressure = read_pressure_only(AMBIENT_PRESSURE_SENSOR);
    ambient_counter = 1;

    // experimentally we will report everything in the stream from
    // both sensor; sadly the BM# 680 is to slow to do this every sample.
    sensor_report_full(AIRWAY_PRESSURE_SENSOR);
    sensor_report_full(AMBIENT_PRESSURE_SENSOR);

    if (ambient_pressure != SENSOR_SENTINEL) {
      output_measurement('M', 'P', 'B', 1, sample_ms, ambient_pressure);

      ambient_window[amb_wc] = ambient_pressure;
      amb_wc = (amb_wc + 1) % AMB_WINDOW_SIZE;
    } else {
      Serial.print("\"NA\"");
    }
  } else {
    ambient_counter++;
  }

  // Smooth the ambient pressure reading
  signed long smooth_ambient = 0;
  for (int i = 0; i < AMB_WINDOW_SIZE; i++) {
    smooth_ambient += ambient_window[i];
  }
  smooth_ambient = (signed long) (smooth_ambient / AMB_WINDOW_SIZE);

  // Read airway pressure
  if (pressure_internal != SENSOR_SENTINEL) {
    output_measurement('M', 'P', 'A', 0, sample_ms, pressure_internal); // eg. 10444

    // really this should be a running max, for now it is instantaneous TODO: improve!
    pressure_max_display = pressure_internal - smooth_ambient;
    output_measurement('M', 'D', 'A', 0, sample_ms, pressure_internal - smooth_ambient);
  } else {
    // This is not actually part of the format!!!
    Serial.print("\"NA\"");
  }
}

void pressure_init(){
  init_sensor(AIRWAY_PRESSURE_SENSOR);
  init_sensor(AMBIENT_PRESSURE_SENSOR);
  
  signed long v = read_pressure_only(AMBIENT_PRESSURE_SENSOR);
  init_ambient(v);
}
