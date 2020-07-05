#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14

unsigned long on_press_button_c_press_ms = 0;


void init_inputs(){
	 // Setup the 3 buttons.
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
}

void on_press_button_a() {
  Serial.println("A BUTTON");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("A");
}

void on_press_button_b() {
  Serial.println("B BUTTON");
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
  Serial.println();
}
