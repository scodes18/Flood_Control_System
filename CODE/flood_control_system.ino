#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// I2C LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins
const int trigPin   = 9;
const int echoPin   = 10;
const int greenLED  = 4;
const int yellowLED = 5;
const int redLED    = 6;
const int buzzerPin = 7;

// Variables
long duration;
int distance;

// Warning timing
unsigned long warningStartTime = 0;
bool warningDone = false;

const unsigned long warningDuration = 3000; // 3 seconds

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  noTone(buzzerPin);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Dam Monitoring");
}

void loop() {

  // Ultrasonic trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) {
    noTone(buzzerPin);
    return;
  }

  distance = (duration * 0.034) / 2;

  lcd.setCursor(0, 1);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print(" cm   ");

  // ================= SAFE (>20 cm) =================
  if (distance > 20) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);

    noTone(buzzerPin);

    warningStartTime = 0;
    warningDone = false;

    lcd.setCursor(10, 1);
    lcd.print("SAFE  ");
  }

  // ================= WARNING (10–20 cm) =================
  else if (distance > 10) {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, HIGH);
    digitalWrite(redLED, LOW);

    lcd.setCursor(10, 1);
    lcd.print("WARN  ");

    // Start warning beep once
    if (!warningDone) {
      if (warningStartTime == 0) {
        warningStartTime = millis();
      }

      // Slow warning beep
      tone(buzzerPin, 800);   // Warning sound
      delay(200);
      noTone(buzzerPin);
      delay(200);

      if (millis() - warningStartTime >= warningDuration) {
        noTone(buzzerPin);
        warningDone = true;
      }
    }
  }

  // ================= DANGER (≤10 cm) =================
  else {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, HIGH);

    // Continuous high-priority danger alarm
    tone(buzzerPin, 2000);   // Danger sound

    warningStartTime = 0;
    warningDone = false;

    lcd.setCursor(10, 1);
    lcd.print("DANGER");
  }

  delay(200);
}
