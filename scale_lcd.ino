/*
   HX711 Load Cell Example - With 16x2 I2C LCD
*/

#include <HX711_ADC.h>
#include <LiquidCrystal_I2C.h>
#if defined(ESP8266) || defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

// HX711 Pins
const int HX711_dout = 4;
const int HX711_sck = 5;

// HX711 object
HX711_ADC LoadCell(HX711_dout, HX711_sck);

// LCD object (address 0x27, 16 cols, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Calibration value
float calibrationValue = 2165.0;

unsigned long t = 0;

void setup() {
  Serial.begin(57600);
  delay(10);

  // Init LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Load Cell Ready");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  Serial.println("Starting...");

  LoadCell.begin();

  unsigned long stabilizingtime = 2000;
  boolean _tare = true;

  LoadCell.start(stabilizingtime, _tare);

  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check wiring!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Timeout!");
    lcd.setCursor(0, 1);
    lcd.print("Check wiring...");
    while (1);
  } else {
    LoadCell.setCalFactor(calibrationValue);
    Serial.println("Startup complete");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scale Ready!");
    lcd.setCursor(0, 1);
    lcd.print("Send 't' to tare");
    delay(1500);
  }
}

void loop() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 500;

  // Update load cell
  if (LoadCell.update()) newDataReady = true;

  // Print data
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float weight = LoadCell.getData();

      // Serial output
      Serial.print("Weight (g): ");
      Serial.println(weight);

      // LCD output
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Weight:");
      lcd.setCursor(0, 1);
      lcd.print(weight, 1);  // 1 decimal place
      lcd.print(" g");

      newDataReady = 0;
      t = millis();
    }
  }

  // Manual tare via Serial
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell.tareNoDelay();
      Serial.println("Taring...");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Taring...");
    }
  }

  // Tare complete check
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tare Complete!");
    delay(1000);
  }
}