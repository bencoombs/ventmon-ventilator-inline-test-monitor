#define PIRDS_SENSIRION_SFM3200 0
#define PIRDS_SENSIRION_SFM3400 1

SFM3X00 flow_sensor(0x40);

char* flow_too_high = FLOW_TOO_HIGH;
char* flow_too_low = FLOW_TOO_LOW;

int sensirion_sensor_type = PIRDS_SENSIRION_SFM3200;
// At present we have to install the SENSIRION_SFM3400 backwards
// because of the physical mounting!!!
bool SENSOR_INSTALLED_BACKWARD = true;

void init(){
  flow_sensor.begin();	
}

void read_flow_sensor(uint32_t sample_ms) {
  // FLOW
  // Units for flowrate are slm * 1000, or milliliters per minute.
  float raw_flow = -999.0;
  raw_flow = flow_sensor.readFlow();
  if (flow_sensor.checkRange(raw_flow)) {
    if (raw_flow > 0) {
      output_meta_event( (char *) ((SENSOR_INSTALLED_BACKWARD) ? flow_too_low : flow_too_high), sample_ms);
    } else {
      output_meta_event( (char *) ((SENSOR_INSTALLED_BACKWARD) ? flow_too_high : flow_too_low), sample_ms);
    }
  }

  float flow = (SENSOR_INSTALLED_BACKWARD) ? -raw_flow : raw_flow;
  signed long flow_milliliters_per_minute = (signed long) (flow * 1000);
  output_measurement('M', 'F', 'A', 0, sample_ms, flow_milliliters_per_minute);
}
