#ifndef DISPLAY_H
#define DISPLAY_H

void display_init();
void update_display();
void clear_display();
//void display_pirds();
void display_print_line(int line, char* s);
void display_min_max_pressure(bool max_not_min, long display_pressure);
void display_pressure(long display_airway_pressure);
void display_debug(signed long display_debug_value);
void display_oxygen(signed long fiO2);
void display_print_pirds(char e, char t, char loc, unsigned short int n, unsigned long ms, signed long val);
void display_ethernet_starting();
void display_ethernet_connected(bool is_connected);

#endif
