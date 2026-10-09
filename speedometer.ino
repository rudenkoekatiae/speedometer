#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET      -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TRIG1 2
#define ECHO1 3
#define TRIG2 4
#define ECHO2 5

#define PWR1  6
#define PWR2  7

const float          DISTANCE_M    = 0.25;
const int            DETECT_CM     = 15;
const long           TIMEOUT_US    = 25000;
const unsigned long  RESET_MS      = 3000;

bool          oledOk        = false;
bool          s1Triggered   = false;
bool          s2Triggered   = false;
unsigned long time1         = 0;
unsigned long time2         = 0;
float         lastSpeed     = 0.0;
unsigned long lastTriggerMs = 0;
unsigned long lastDebugMs   = 0;
const int     DEBUG_MS      = 200;

float readCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long dur = pulseIn(echoPin, HIGH, TIMEOUT_US);
  if (dur == 0) return 999.0;
  return dur * 0.0343 / 2.0;
}

void showDisplay(float speedMS, bool waiting) {
  if (!oledOk) return;
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  if (waiting) {
    display.setTextSize(1);
    display.setCursor(20, 8);
    display.print("Waiting...");
    display.setCursor(4, 28);
    display.print("Sensor 1: ");
    display.print(s1Triggered ? "TRIGGERED" :);
    display.setCursor(4, 44);
    display.print("Sensor 2:");
  } else if (speedMS == 0) {
    display.setTextSize(1);
    display.setCursor(20, 24);
    display.print("Ready");
  } else {
    display.setTextSize(2);
    int xPos = (speedMS < 10.0) ? 36 : (speedMS < 100.0) ? 24 : 8;
    display.setCursor(xPos, 12);
    display.print(speedMS, 2);
    display.setTextSize(1);
    display.setCursor(36, 50);
    display.print("m / s");
  }
  display.display();
}

void setup() {
  Serial.begin(9600);

  pinMode(PWR1, OUTPUT);
  pinMode(PWR2, OUTPUT);
  digitalWrite(PWR1, LOW);
  digitalWrite(PWR2, LOW);

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  digitalWrite(TRIG1, LOW);
  digitalWrite(TRIG2, LOW);

  delay(500);

  Wire.begin();
  delay(200);

  oledOk = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (!oledOk) oledOk = display.begin(SSD1306_SWITCHCAPVCC, 0x3D);

  if (oledOk) {
    Serial.println("OLED OK!");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(12, 20);
    display.print("Speed meter ready");
    display.setCursor(28, 38);
    display.print("d = 25 cm");
    display.display();
    delay(1000);
  } else {
    Serial.println("OLED FAILED — running without display.");
  }

  digitalWrite(PWR1, HIGH);
  digitalWrite(PWR2, HIGH);
  delay(100); // let sensors stabilise

  Serial.println("Speed meter started");
  Serial.println("S1: cm  |  S2: cm  (every 200 ms)");

  showDisplay(0, false);
}

void loop() {
  float d1 = readCM(TRIG1, ECHO1);
  float d2 = readCM(TRIG2, ECHO2);

  if (millis() - lastDebugMs >= DEBUG_MS) {
    lastDebugMs = millis();
    Serial.print("S1: ");
    Serial.print(d1, 1);
    Serial.print(" cm  |  S2: ");
    Serial.print(d2, 1);
    Serial.println(" cm");
  }

  if (!s1Triggered && d1 < DETECT_CM) {
    s1Triggered   = true;
    time1         = micros();
    lastTriggerMs = millis();
    Serial.println(">> Sensor 1 TRIGGERED");
    showDisplay(0, true);
  }

  if (s1Triggered && !s2Triggered && d2 < DETECT_CM) {
    s2Triggered = true;
    time2       = micros();

    unsigned long deltaUS = time2 - time1;
    if (deltaUS > 0) {
      float deltaSec = deltaUS / 1000000.0;
      lastSpeed      = DISTANCE_M / deltaSec;

      Serial.print(">> Sensor 2 TRIGGERED  |  Time: ");
      Serial.print(deltaSec * 1000.0, 2);
      Serial.print(" ms  |  Speed: ");
      Serial.print(lastSpeed, 2);
      Serial.println(" m/s");

      showDisplay(lastSpeed, false);
    }
  }

  if (s1Triggered && !s2Triggered &&
      millis() - lastTriggerMs > RESET_MS) {
    Serial.println("!! Timeout - sensor 2 never triggered. Resetting.");
    s1Triggered = false;
    s2Triggered = false;
    showDisplay(lastSpeed, false);
  }

  if (s1Triggered && s2Triggered &&
      millis() - lastTriggerMs > RESET_MS) {
    s1Triggered = false;
    s2Triggered = false;
    showDisplay(lastSpeed, false);
  }

  delay(20);
}
