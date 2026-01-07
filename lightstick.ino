const byte BTN = 2;

const byte PIN_R = 9;
const byte PIN_G = 10;
const byte PIN_B = 11;

const unsigned long DEBOUNCE_MS  = 50;
const unsigned long LONGPRESS_MS = 800;

byte mode = 0;
const byte MODE_COUNT = 3;

bool ledEnabled = true;

// Button state tracking
bool lastRead = LOW;
bool stableState = LOW;
unsigned long lastDebounceTime = 0;

unsigned long pressStart = 0;
bool longFired = false;

// If your button works opposite, change HIGH to LOW
const byte PRESSED = HIGH;

// ---------- Common anode RGB helpers ----------
void rgbOff() {
  digitalWrite(PIN_R, HIGH);
  digitalWrite(PIN_G, HIGH);
  digitalWrite(PIN_B, HIGH);
}

void rgbPWM(byte r, byte g, byte b) {
  if (!ledEnabled) { rgbOff(); return; }
  analogWrite(PIN_R, 255 - r);
  analogWrite(PIN_G, 255 - g);
  analogWrite(PIN_B, 255 - b);
}

// ---------- Rainbow math ----------
void wheel(byte pos, byte &r, byte &g, byte &b) {
  if (pos < 85) {
    r = 255 - pos * 3;
    g = pos * 3;
    b = 0;
  } else if (pos < 170) {
    pos -= 85;
    r = 0;
    g = 255 - pos * 3;
    b = pos * 3;
  } else {
    pos -= 170;
    r = pos * 3;
    g = 0;
    b = 255 - pos * 3;
  }
}

// ---------- MODES ----------

// 0) Blinking Rainbow
void modeBlinkRainbow() {
  static byte hue = 0;
  static bool on = true;
  static unsigned long t = 0;

  const unsigned long BLINK_MS = 140;

  if (millis() - t >= BLINK_MS) {
    t = millis();
    on = !on;
  }

  if (!on) {
    rgbOff();
    delay(8);
    return;
  }

  byte r,g,b;
  wheel(hue, r, g, b);

  r = (byte)(r * 0.75);
  g = (byte)(g * 0.75);
  b = (byte)(b * 0.75);

  rgbPWM(r,g,b);

  hue++;
  delay(12);
}

// 1) Solid LIGHT RED
void modeSolidLightRed() {
  rgbPWM(255, 40, 0);   // ← LIGHT RED
  delay(20);
}

// 2) Blinking LIGHT RED
void modeBlinkLightRed() {
  static bool on = false;
  static unsigned long t = 0;

  const unsigned long BLINK_MS = 320;

  if (millis() - t >= BLINK_MS) {
    t = millis();
    on = !on;
    if (on) rgbPWM(255, 40, 0);
    else rgbOff();
  }
  delay(10);
}

// ---------- Button handling ----------
void handleButton() {
  bool reading = digitalRead(BTN);

  if (reading != lastRead) {
    lastDebounceTime = millis();
    lastRead = reading;
  }

  if (millis() - lastDebounceTime > DEBOUNCE_MS) {
    if (stableState != reading) {
      stableState = reading;

      if (stableState == PRESSED) {
        pressStart = millis();
        longFired = false;
      } else {
        if (!longFired) {
          mode = (mode + 1) % MODE_COUNT;
          rgbOff();
          delay(60);
        }
      }
    }
  }

  if (stableState == PRESSED && !longFired) {
    if (millis() - pressStart >= LONGPRESS_MS) {
      longFired = true;
      ledEnabled = !ledEnabled;
      if (!ledEnabled) rgbOff();
      delay(120);
    }
  }
}

void setup() {
  pinMode(BTN, INPUT);   // button module with VCC
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  rgbOff();
}

void loop() {
  handleButton();

  if (!ledEnabled) {
    rgbOff();
    delay(10);
    return;
  }

  switch (mode) {
    case 0: modeBlinkRainbow();   break;
    case 1: modeSolidLightRed();  break;
    case 2: modeBlinkLightRed();  break;
  }
}
