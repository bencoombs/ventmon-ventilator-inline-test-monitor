#include <PIRDS.h>
#include <Adafruit_ADS1015.h>
#include "data_tx.h"


// oxygen sensor is connected to channel 3 of ADS115
#define O2_CHANNEL        3
#define UNPLUGGED_MAX    400
#define FIO2_SAMPLE_RATE 4000

Adafruit_ADS1115 ads;

int initial_O2_sample = 0;
bool oxygen_sensing = true;
unsigned long last_fiO2_sample = 0;
float fiO2_sample = 0;
float initialO2;

double average_adc(int value)
{
  double adc = 0;
  double average;

  for (int i = 0; i <= 19; i++)
  {
    int16_t adsread = ads.readADC_SingleEnded(value);
    adc = adc + adsread;
    delay(10);
  }

  average = adc / 20;

  return average;
}

void oxygen_sensor_init(){
	// setup ADC for oxygen sensing
  ads.setGain(GAIN_SIXTEEN);
  ads.begin();
  initialO2 = average_adc(O2_CHANNEL);

  Serial.println(-(UNPLUGGED_MAX));

  if (initialO2 <= UNPLUGGED_MAX && initialO2 >= -(UNPLUGGED_MAX))
  {
    Serial.println("WARNING: No oxygen sensor detected.");
    oxygen_sensing = false;
  }
}


// Read Oxygen Sensor - FiO2
void read_oxygen_sensor(uint32_t sample_ms) {
  unsigned long fiO2_timer = millis();

  // Sample every few seconds
  if (oxygen_sensing && ((fiO2_timer -  last_fiO2_sample) >= FIO2_SAMPLE_RATE))
  {
    float fiO2 = (average_adc(O2_CHANNEL) / initial_O2_sample) * 20.9;
    output_measurement('M', 'O', 'A', 0, sample_ms, fiO2);
    last_fiO2_sample = millis();

    fiO2_sample = fiO2;
  }
}
