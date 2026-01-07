const int LED_PIN = 3;   
const int BTN_PIN = 2;

int mode = 0;
int lastButtonState = HIGH;

unsigned long lastBlinkTime = 0;
bool ledState = HIGH;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, LOW); // OFF (common anode)
}

void loop() {
  int buttonState = digitalRead(BTN_PIN);

  // Detect button press
  if (lastButtonState == HIGH && buttonState == LOW) {
    mode++;
    if (mode > 3) mode = 0;
    delay(200); // debounce
  }
  lastButtonState = buttonState;

  unsigned long now = millis();

  switch (mode) {

    case 0: // Steady ON
      digitalWrite(LED_PIN, LOW); // ON (common anode)
      break;

    case 1: // Slow blink
      if (now - lastBlinkTime >= 1000) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? LOW : HIGH);
        lastBlinkTime = now;
      }
      break;

    case 2: // Medium blink
      if (now - lastBlinkTime >= 500) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? LOW : HIGH);
        lastBlinkTime = now;
      }
      break;

    case 3: // Fast blink
      if (now - lastBlinkTime >= 200) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? LOW : HIGH);
        lastBlinkTime = now;
      }
      break;
  }
}