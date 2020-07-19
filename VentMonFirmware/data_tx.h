#ifndef DATA_TX_H
#define DATA_TX_H

void serial_print_pirds(char e, char t, char loc, unsigned short int n, unsigned long ms, signed long val);
void output_measurement(char e, char t, char loc, unsigned short int n, unsigned long ms, signed long val);
bool send_data(char event, char mtype, char loc, uint8_t num, uint32_t ms, int32_t value);
void output_meta_event(char *msg, unsigned long ms);
Measurement get_measurement(char event, char mtype, char loc, uint8_t num, uint32_t ms, int32_t value);
Message get_message(uint32_t ms, char *msg);

#endif
