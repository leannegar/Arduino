const byte RGB_R = 9;
const byte RGB_G = 10;
const byte RGB_B = 11;

const byte BTN1 = 2;   // pet button (or one of them)
const byte BTN2 = 3;   // optional
const byte BTN3 = 4;   // optional

// ----------- Toy parameters -----------
int happiness = 70;                 // 0..100
const int MAX_HAPPY = 100;
const int MIN_HAPPY = 0;

const int PET_GAIN = 8;             // how much happiness increases per pet
const unsigned long TOUCH_GRACE_MS = 5000;   // "untouched for a while" threshold (5s)
const unsigned long DECAY_STEP_MS  = 1500;   // after grace, lose points every 1.5s
const int DECAY_AMOUNT = 3;         // points lost per decay step

// ----------- Timing -----------
unsigned long lastTouchMs = 0;
unsigned long lastDecayMs = 0;

// ----------- Button polarity auto-detect -----------
byte PRESSED = HIGH; // will be set in setup()

bool readPressed(byte pin) {
  return digitalRead(pin) == PRESSED;
}

bool anyPetPressedRaw() {
  // "pressed" state uses PRESSED variable (after detection)
  return readPressed(BTN1) || readPressed(BTN2) || readPressed(BTN3);
}

void detectButtonPolarity() {
  // Read idle state of BTN1 (assumes not pressed at boot)
  byte idle = digitalRead(BTN1);
  // pressed is opposite of idle
  PRESSED = (idle == HIGH) ? LOW : HIGH;
}

// ----------- Common-anode RGB helpers -----------
void rgbOff() {
  digitalWrite(RGB_R, HIGH);
  digitalWrite(RGB_G, HIGH);
  digitalWrite(RGB_B, HIGH);
}

// PWM set (0..255), common anode invert
void rgbPWM(byte r, byte g, byte b) {
  analogWrite(RGB_R, 255 - r);
  analogWrite(RGB_G, 255 - g);
  analogWrite(RGB_B, 255 - b);
}

// Mood display based on happiness
void showMood() {
  // Very Sad: blinking red
  if (happiness <= 15) {
    static bool on = false;
    static unsigned long t = 0;
    if (millis() - t > 350) {
      t = millis();
      on = !on;
    }
    if (on) rgbPWM(255, 0, 0);
    else rgbOff();
    return;
  }

  // Sad: light red
  if (happiness <= 40) {
    rgbPWM(255, 40, 0);   // soft red (sad)
    return;
  }

  // Okay/Calm: cyan/blue
  if (happiness <= 70) {
    rgbPWM(0, 160, 255);  // calm cyan-blue
    return;
  }

  // Happy: green
  rgbPWM(0, 255, 0);
}

// Simple edge-detected “pet” (one pet per click)
bool lastPetState = false;

bool petEvent() {
  bool now = anyPetPressedRaw();
  bool event = (now && !lastPetState);
  lastPetState = now;
  return event;
}

void clampHappiness() {
  if (happiness > MAX_HAPPY) happiness = MAX_HAPPY;
  if (happiness < MIN_HAPPY) happiness = MIN_HAPPY;
}

void onPet() {
  happiness += PET_GAIN;
  clampHappiness();
  lastTouchMs = millis();

  // cute feedback: quick sparkle (white flash)
  rgbPWM(255, 255, 255);
  delay(70);
}

void decayIfUntouched() {
  unsigned long now = millis();

  // only start decaying after grace period
  if (now - lastTouchMs < TOUCH_GRACE_MS) return;

  // decay step timing
  if (now - lastDecayMs >= DECAY_STEP_MS) {
    lastDecayMs = now;
    happiness -= DECAY_AMOUNT;
    clampHappiness();
  }
}

void setup() {
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  rgbOff();

  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
  pinMode(BTN3, INPUT);

  delay(50);
  detectButtonPolarity();

  lastTouchMs = millis();
  lastDecayMs = millis();

  // startup mood intro
  rgbPWM(0, 0, 255); delay(120);
  rgbPWM(0, 255, 0); delay(120);
  rgbOff(); delay(100);
}

void loop() {
  // Pet detected
  if (petEvent()) {
    onPet();
  }

  // Untouched decay
  decayIfUntouched();

  // Show mood continuously
  showMood();

  delay(10);
}
