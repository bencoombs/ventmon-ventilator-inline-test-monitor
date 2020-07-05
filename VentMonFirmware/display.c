// Adafruit OLED FeatherWing
// https://www.adafruit.com/product/2900
// 128x32 OLED

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DEBUG_SERIAL true

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

bool display_connected = false;

void display_init() {
  #ifdef DEBUG_SERIAL
    Serial.println("OLED FeatherWing initializing");
  #endif
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  int ret_oled = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  //Serial.print("Return value for OLED");
  //Serial.println(ret_oled);
  if (ret_oled != 1) { // Address 0x3C for 128x32
    Serial.println("OLED display failed to start! Check it is connected.");
    // TODO: send message via network stream
    display_connected = false;
    return;
  }

  display_connected = true;
  Serial.println("OLED display started!");

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
{
  // TODO: add OLED output
  switch (t) {
    case 'P':
      display_airway_pressure = val;
      break;
    case 'F':
      display_debug_value = val;
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

void clear_display() {
  display.clearDisplay();
  display.display();
}

void display_min_max_pressure(bool max_not_min) {
  display.print(max_not_min ? "Pmax: " : "Pmin: " );
  char buffer[32]; // 32 Chars per line on the OLED
  long display_pressure = max_not_min ? pressure_max_display : pressure_min_display;
  int pressure_sign = ( display_pressure < 0) ? -1 : 1;
  int abs_pressure = (pressure_sign == -1) ? - display_pressure : display_pressure;
  sprintf(buffer, "%d.1", display_pressure / 10);
  display.print(buffer);
  display.println(" cmH2O");

  Serial.println(buffer);
}

void display_pressure() {
  display.print("Pressure: ");
  display.print(display_airway_pressure);
  display.println(" cmH20");
}

void display_debug() {
  display.print("Debug: ");
  display.println(display_debug_value);
}

void display_oxgen() {
  display.print("FiO2: ");
  display.print(fiO2_sample);
  display.println(" %");
}

void update_display() {
  display.clearDisplay();

  display.setCursor(0, 0);
  display_pressure();
  display_debug();
  //display_min_max_pressure(true);
  //display.setCursor(0, 10);
  //display_min_max_pressure(false);
  
  display.setCursor(0, 20);
  display_oxgen();

  display.display();
}
