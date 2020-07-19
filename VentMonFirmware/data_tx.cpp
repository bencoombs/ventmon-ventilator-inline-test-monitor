#include <PIRDS.h>
#include <cstring>
#include "display.h"
#include "networking.h"

// Set true to show PIRDS streaming data in the Serial Monitor window.
#define DEBUG_SERIAL true

Measurement get_measurement(char event, char mtype, char loc, uint8_t num, uint32_t ms, int32_t value) {
  Measurement ma;
  ma.event = event;
  ma.type = mtype;
  ma.loc = loc;
  ma.num = 0 + num;
  ma.ms = ms;
  ma.val = value;
  return ma;
}

Message get_message(uint32_t ms, char *msg) {
  Message m;
  m.event = 'E';
  m.type = 'M';
  m.ms = ms;
  int n = strlen(msg);
  if (n > 255) {
    m.b_size = 255;
    memcpy(m.buff, msg, 255);
  } else {
    m.b_size = n;
    strcpy(m.buff, msg);
  }
  return m;
}

void output_meta_event(char *msg, unsigned long ms) {
  Message m = get_message(ms, msg);
  char buff[264];
  int rv = fill_JSON_buffer_message(&m, buff, 264);

  #ifdef DEBUG_SERIAL
//    Serial.print(buff);
  //  Serial.println();
  #endif

  send_data_message(m);
}


void serial_print_pirds(char e, char t, char loc, unsigned short int n, unsigned long ms, signed long val) {
  Measurement ma = get_measurement(e, t, loc, n, ms, val);
  // I need a proof that this buffer is larger enough, but I think it is...
  // TODO: confirm buffer is large enough
  char buff[256];
  int rv = fill_JSON_buffer_measurement(&ma, buff, 256);

  #ifdef DEBUG_SERIAL
//    Serial.print(buff);
  //  Serial.println();
  #endif
}


bool send_data(char event, char mtype, char loc, uint8_t num, uint32_t ms, int32_t value) {
  Measurement ma = get_measurement(event, mtype, loc, num, ms, value);
  return send_data_measurement(ma);
}


void output_measurement(char e, char t, char loc, unsigned short int n, unsigned long ms, signed long val) {
  display_print_pirds(e, t, loc, n, ms, val);
  
  serial_print_pirds(e, t, loc, n, ms, val);

  send_data(e, t, loc, n, ms, val);
}
