#ifndef DEBUG_SERIAL_H
#define DEBUG_SERIAL_H 
/*
#define print(...) Serial.print(__VA_ARGS__);
#define println(...) Serial.println(__VA_ARGS__);
#define flush() Serial.flush();
*/
void error(int level, char* message);

#endif
