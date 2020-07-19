#ifndef PRESSURE_H
#define PRESSURE_H

void sensor_check_status();
void pressure_init();
void init_sensor(int i);
void init_ambient(signed long v);
signed long read_pressure_only(int i);
void read_pressure_sensors(uint32_t sample_ms);

#endif
