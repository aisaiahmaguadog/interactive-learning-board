#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include "fonts/Arial14.h"   // larger readable font

// ---- Joystick pins ----
#define JOY_X   A0
#define JOY_Y   A1
#define JOY_SW  13           // joystick button (LOW when pressed)

// ---- Speaker pin ----
#define SPEAKER_PIN 2        // piezo buzzer

// ---- LED pins ----
const int green = 22;        // success LED
const int red   = 23;        // fail LED

// ---- OLED setup ----
SSD1306AsciiWire oled;
#define OLED_ADDR 0x3C

// ---- Menu Items ----
const char* menuItems[] = {"Resistor", "Diode", "Capacitor"};
const int MENU_SIZE = sizeof(menuItems) / sizeof(menuItems[0]);

// ---- App State Machine ----
enum ScreenState { SCREEN_MENU, SCREEN_LEVEL1, SCREEN_LEVEL2, SCREEN_LEVEL3 };
ScreenState screen = SCREEN_MENU;

// ---- Menu State ----
int menuIndex = 0;

// ---- Joystick thresholds ----
const int JOY_DEADZONE = 150;
const int JOY_LOW_TH   = 512 - JOY_DEADZONE;
const int JOY_HIGH_TH  = 512 + JOY_DEADZONE;

// ---- Scroll cooldown ----
const unsigned long NAV_COOLDOWN_MS = 180;
unsigned long lastNavMs = 0;

// ---- Button debounce ----
const unsigned long DEBOUNCE_MS = 40;
bool btnStable = HIGH;
bool btnLastRead = HIGH;
unsigned long btnLastChangeMs = 0;

// ---- Debounced button event ----
bool buttonPressedEvent() {
  unsigned long now = millis();
  bool raw = digitalRead(JOY_SW);

  if (raw != btnLastRead) {
    btnLastRead = raw;
    btnLastChangeMs = now;
  }

  static bool lastReported = HIGH;
  if ((now - btnLastChangeMs) >= DEBOUNCE_MS) {
    if (btnStable != raw) {
      btnStable = raw;
      if (btnStable == LOW && lastReported == HIGH) {
        lastReported = LOW;
        return true;
      }
      lastReported = btnStable;
    }
  }
  return false;
}

// ---- Sounds ----
void clickScroll() { tone(SPEAKER_PIN, 1500, 25); }
void clickSelect() { tone(SPEAKER_PIN, 2000, 40); }

// ---- Forward Declarations ----
void drawMenu();
void handleMenuInputs();
void runLevel1();
void runLevel2();
void runLevel3();
void playSuccessJingle();

// ============================================================
//                         SETUP
// ============================================================
void setup() {
  Wire.begin();
  oled.begin(&Adafruit128x64, OLED_ADDR);
  oled.setFont(Arial14);
  oled.clear();

  pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(SPEAKER_PIN, OUTPUT);

  // LED pin init (global)
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  digitalWrite(green, LOW);
  digitalWrite(red, LOW);

  drawMenu();
}

// ============================================================
//                          LOOP
// ============================================================
void loop() {
  switch (screen) {
    case SCREEN_MENU:   handleMenuInputs(); break;
    case SCREEN_LEVEL1: runLevel1(); break;
    case SCREEN_LEVEL2: runLevel2(); break;
    case SCREEN_LEVEL3: runLevel3(); break;
  }
}

// ============================================================
//                          MENU
// ============================================================
void drawMenu() {
  oled.clear();
  oled.println("Menu:");
  for (int i = 0; i < MENU_SIZE; i++) {
    if (i == menuIndex) oled.print("> ");
    else                oled.print("  ");
    oled.println(menuItems[i]);
  }
}

void handleMenuInputs() {
  unsigned long now = millis();
  int y = analogRead(JOY_Y);

  if (now - lastNavMs >= NAV_COOLDOWN_MS) {
    // Inverted up/down
    if (y < JOY_LOW_TH) {
      menuIndex = (menuIndex + 1) % MENU_SIZE;
      lastNavMs = now;
      clickScroll();
      drawMenu();
    }
    else if (y > JOY_HIGH_TH) {
      menuIndex = (menuIndex - 1 + MENU_SIZE) % MENU_SIZE;
      lastNavMs = now;
      clickScroll();
      drawMenu();
    }
  }

  if (buttonPressedEvent()) {
    clickSelect();
    if (menuIndex == 0) screen = SCREEN_LEVEL1;
    else if (menuIndex == 1) screen = SCREEN_LEVEL2;
    else if (menuIndex == 2) screen = SCREEN_LEVEL3;
  }
}

// ============================================================
//                       LEVEL 1 (Resistor)
// ============================================================

const int Vo = A2;
float L1TargetArray[] = {1.66, 3.58, 4.15};
int L1Index;
float L1Target;
const float L1_TOLERANCE = 0.1;

void playSuccessJingle() {
  tone(SPEAKER_PIN, 784, 180); delay(200);
  tone(SPEAKER_PIN, 659, 180); delay(200);
  tone(SPEAKER_PIN, 523, 180); delay(200);
  tone(SPEAKER_PIN, 659, 180); delay(200);
  tone(SPEAKER_PIN, 784, 300); delay(350);
  noTone(SPEAKER_PIN);
}

void runLevel1() {
  static bool firstRun = true;
  static unsigned long lastUpdate = 0;

  if (firstRun) {
    randomSeed(analogRead(A7));
    L1Index = random(0, 3);
    L1Target = L1TargetArray[L1Index];

    pinMode(Vo, INPUT);
    pinMode(green, OUTPUT);
    pinMode(red, OUTPUT);
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);

    oled.clear();
    oled.println("LEVEL 1");
    oled.print("Target: ");
    oled.print(L1Target, 2);
    oled.println(" V");
    oled.println();
    oled.println("Now:    --.--V");
    oled.println();
    oled.println("Press for menu");

    firstRun = false;
  }

  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    float Vin = (analogRead(Vo) * 5.0) / 1023.0;

    oled.setCursor(0, 4);
    oled.print("Now:    ");
    oled.print(Vin, 2);
    oled.print("V   ");

    if (abs(Vin - L1Target) <= L1_TOLERANCE) {
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      clickSelect();
      oled.clear();
      oled.print("Great job!");
      oled.println();
      playSuccessJingle();
      oled.println("Returning...");
      delay(800);
      digitalWrite(green, LOW);
      firstRun = true;
      screen = SCREEN_MENU;
      drawMenu();
      return;
    }

    if (Vin != 5) digitalWrite(red, HIGH);
    else          digitalWrite(red, LOW);
  }

  if (buttonPressedEvent()) {
    clickSelect();
    digitalWrite(red, LOW);
    firstRun = true;
    screen = SCREEN_MENU;
    drawMenu();
  }
}

// ============================================================
//                       LEVEL 2 (Diode)
// ============================================================

const int L2_PIN = A2;
const float L2_TARGET = 2.68;
const float L2_TOL = 0.10;

void runLevel2() {
  static bool firstRun = true;
  static unsigned long lastUpdate = 0;

  if (firstRun) {
    pinMode(L2_PIN, INPUT);
    pinMode(green, OUTPUT);
    pinMode(red, OUTPUT);
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);

    oled.clear();
    oled.println("LEVEL 2");
    oled.print("Target: ");
    oled.print(L2_TARGET, 2);
    oled.println(" V");
    oled.println();
    oled.println("Now:    --.--V");
    oled.println();
    oled.println("Btn=exit");

    firstRun = false;
  }

  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    float Vin = (analogRead(L2_PIN) * 5.0) / 1023.0;

    oled.setCursor(0, 4);
    oled.print("Now:    ");
    oled.print(Vin, 2);
    oled.print("V   ");

    if (abs(Vin - L2_TARGET) <= L2_TOL) {
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      oled.clear();
      oled.println("GOOD JOB!");
      oled.println();
      oled.println("Returning...");
      playSuccessJingle();
      digitalWrite(green, LOW);
      delay(800);
      firstRun = true;
      screen = SCREEN_MENU;
      drawMenu();
      return;
    }
  }

  if (buttonPressedEvent()) {
    clickSelect();
    digitalWrite(red, LOW);
    firstRun = true;
    screen = SCREEN_MENU;
    drawMenu();
  }
}

// ============================================================
//                       LEVEL 3 (Capacitor)
// ============================================================
// =======================
//        LEVEL 3
//  Capacitor Discharge Test (retry on fail)
// =======================

const int L3_PIN = A2;                 // analog input for capacitor
const float L3_START_THRESHOLD = 2.00; // timing begins when voltage >= this
const float L3_END_THRESHOLD   = 0.20; // timing stops when voltage <= this
const unsigned long L3_WIN_TIME_MS = 500;  // >500ms = big cap = success
const unsigned long L3_UPDATE_MS = 500;    // OLED refresh interval

// small helper to redraw the waiting screen without re-running firstRun branch
static void L3_showWaiting() {
  oled.clear();
  oled.println("LEVEL 3");
  oled.println("Connect charged cap");
  oled.println("to A2.");
  oled.println();
  oled.println("Waiting for >2.00V...");
}

void runLevel3() {
  static bool firstRun = true;
  static unsigned long tStart = 0;
  static unsigned long lastUpdate = 0;
  static bool timing = false;

  if (firstRun) {
    pinMode(L3_PIN, INPUT);
    pinMode(green, OUTPUT);
    pinMode(red, OUTPUT);
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);

    L3_showWaiting();

    tStart = 0;
    timing = false;
    firstRun = false;
    return;
  }

  // Allow exit before timing starts
  if (buttonPressedEvent() && !timing) {
    clickSelect();
    firstRun = true;
    screen = SCREEN_MENU;
    drawMenu();
    return;
  }

  float Vin = (analogRead(L3_PIN) * 5.0) / 1023.0;
  unsigned long now = millis();

  // keep red off during wait/timing; only used on fail
  digitalWrite(red, LOW);

  // Wait until capacitor is charged (>2.00 V)
  if (!timing) {
    if (Vin >= L3_START_THRESHOLD) {
      timing = true;
      tStart = now;
      lastUpdate = now;
      oled.clear();
      oled.println("Discharging...");
      oled.println();
      oled.println("t = --- ms");
    }
    return;
  }

  // Timing mode: update screen every 500ms
  if (now - lastUpdate >= L3_UPDATE_MS) {
    lastUpdate = now;
    oled.setCursor(0, 2);
    oled.print("t = ");
    oled.print(now - tStart);
    oled.print(" ms   ");
  }

  // End condition: capacitor discharged below threshold
  if (Vin <= L3_END_THRESHOLD) {
    unsigned long total = now - tStart;
    oled.clear();

    if (total > L3_WIN_TIME_MS) {
      // ---- SUCCESS → return to menu (unchanged) ----
      digitalWrite(green, HIGH);
      oled.println("GOOD JOB!");
      oled.println();
      oled.print("Time: ");
      oled.print(total);
      oled.println(" ms");
      oled.println();
      oled.println("Returning...");
      playSuccessJingle();
      digitalWrite(green, LOW);

      firstRun = true;
      timing = false;
      screen = SCREEN_MENU;
      drawMenu();
      return;
    } else {
      // ---- FAIL → stay in level and retry ----
      digitalWrite(red, HIGH);
      oled.println("Not big cap!");
      oled.println();
      oled.print("Time: ");
      oled.print(total);
      oled.println(" ms");
      oled.println();
      oled.println("Try again...");
      delay(1000);              // brief feedback
      digitalWrite(red, LOW);

      // reset state and show waiting screen again
      tStart = 0;
      timing = false;
      L3_showWaiting();
      return;                   // remain in Level 3
    }
  }
}
