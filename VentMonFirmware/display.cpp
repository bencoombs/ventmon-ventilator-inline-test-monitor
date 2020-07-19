// Adafruit OLED FeatherWing
// https://www.adafruit.com/product/2900
// 128x32 OLED

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DEBUG_SERIAL true

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

bool display_connected = false;
signed long display_pressure_val;
signed long display_fiO2_val;
//unsigned long pressure_max_display;
//unsigned long pressure_min_display;

void clear_display() {
  display.clearDisplay();
  display.display();
}


void display_init() {
  #ifdef DEBUG_SERIAL
//    Serial.println("OLED FeatherWing initializing");
  #endif
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  int ret_oled = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  //Serial.print("Return value for OLED");
  //Serial.println(ret_oled);
  if (ret_oled != 1) { // Address 0x3C for 128x32
//    Serial.println("OLED display failed to start! Check it is connected.");
    // TODO: send message via network stream
    display_connected = false;
    return;
  }

  display_connected = true;
  //Serial.println("OLED display started!");

  // Show image buffer on the display hardware.
  // Displays the Adafruit splash screen by default.
  display.display();

  delay(1500); // Show splash screen for 2 seconds

  // Clear the buffer.
  display.clearDisplay();
  display.display();

 
  // VentMon splash screen
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("VentMon"));
  display.setCursor(0, 20);
  display.setTextSize(1);
  display.println();
  display.display();

  delay(3000);
  clear_display();

  delay(2000);
}

void display_print_pirds(char e, char t, char loc, unsigned short int n, unsigned long ms, signed long val) {
  // TODO: add OLED output
  switch (t) {
    case 'P':
      display_pressure_val = val;
      break;
    case 'F':
      display_fiO2_val = val;
      break;
  }
	
}


// Helper function to display lines of text on the OLED screen.
// Max 128 characters per line
void display_print_line(int line, char* s) {
  if ((line >= 0) && (line < 3)) { // 0, 1, 2
    display.setCursor(0, (line) * 10);
    return;
  }

  if ((sizeof(s) > 20)) {
    return;
  }

  display.print(s);
}


void display_min_max_pressure(bool max_not_min, long display_pressure) {
  display.print(max_not_min ? "Pmax: " : "Pmin: " );
  char buffer[32]; // 32 Chars per line on the OLED
  //long display_pressure = max_not_min ? pressure_max_display : pressure_min_display;
  int pressure_sign = ( display_pressure < 0) ? -1 : 1;
  int abs_pressure = (pressure_sign == -1) ? - display_pressure : display_pressure;
  sprintf(buffer, "%d.1", display_pressure / 10);
  display.print(buffer);
  display.println(" cmH2O");

//  Serial.println(buffer);
}

void display_pressure(long display_airway_pressure) {
  display.print("Pressure: ");
  display.print(display_airway_pressure);
  display.println(" cmH20");
}

void display_debug(signed long display_debug_value) {
  display.print("Debug: ");
  display.println(display_debug_value);
}

void display_oxygen(float fiO2) {
  display.print("FiO2: ");
  display.print(fiO2);
  display.println(" %");
}

void display_ethernet_starting(){
  display.setCursor(0, 0);
  display.println(F("Ethernet starting..."));
  display.display();
}

void display_ethernet_connected(bool is_connected){
  display.setCursor(0, 10);
  display.print(F("Ethernet "));
  display.println(is_connected ? "connected" : "offline");
  display.display();
}

void update_display() {
  display.clearDisplay();

  display.setCursor(0, 0);
  display_pressure(display_pressure_val);
  //display_debug();
  //display_min_max_pressure(true);
  //display.setCursor(0, 10);
  //display_min_max_pressure(false);
  
  display.setCursor(0, 20);
  display_oxygen(display_fiO2_val);

  display.display();
}
/*
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14

unsigned long on_press_button_c_press_ms = 0;


void init_inputs(){
   // Setup the 3 buttons.
  pinMode(BUTTON_A, 0);
  pinMode(BUTTON_B, 0);
  pinMode(BUTTON_C, 0);
}

void on_press_button_a() {
//  Serial.println("A BUTTON");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("A");
}

void on_press_button_b() {
//  Serial.println("B BUTTON");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("B");
}

// Experimental feature - save log to file on button C pressed.
void on_press_button_c() {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("C");

  unsigned long ms = millis();
  int n = strlen(SAVE_LOG_TO_FILE);
  char buffer[255];
  strcpy(buffer, SAVE_LOG_TO_FILE);
  strcpy(buffer + n, "test_file_name");
  output_meta_event(buffer, ms);
//  Serial.println();
}*/
