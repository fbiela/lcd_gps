/*
 *
 * Frederico Sales <fbiela@gmail.com>
 * https://www.github.com/fbiela
 * 2017
 * LCD SHIELD + arduino Duemilenove + GPS
 *
 */

 // include
 #include <Arduino.h>
 #include <Wire.h>
 #include <LiquidCrystal.h>
 #include <TinyGPS.h>
 #include <SoftwareSerial.h>
 #include <time.h>

// define
#define BTN A0                  // input to keyboard
#define BTN_RIGHT 0             // btn < 50
#define BTN_UP 1                // btn < 250
#define BTN_DOWN 2              // btn < 450
#define BTN_LEFT 3              // btn < 650
#define BTN_SELECT 4            // btn < 850
#define BTN_NONE 5              // btn < 1000
#define GPS_RX 10               // SoftwareSerial RX GPS
#define GPS_TX 11               // SoftwareSerial TX GPS
#define TIMEZONE -3             // UTC TIMEZONE brazilia -3
#define LCD_00 8                // LCD pin
#define LCD_01 9                // LCD pin
#define LCD_02 4                // LCD pin
#define LCD_03 5                // LCD pin
#define LCD_04 6                // LCD pin
#define LCD_05 7                // LCD pin
#define HARD_SPEED 9600         // serial port speed
#define SOFT_SPEED 38400        // gps speed
#define LCD_LIN 2               // lcd lines (2)
#define LCD_COL 16              // lcd columns (12)
#define D1S 1000                // delay 1s
#define D2S 2000                // delay 2s
#define D3S 3000                // delay 3s
#define D4S 4000                // delay 4s
#define D5S 5000                // delay 5s

// objects
LiquidCrystal lcd(LCD_00, LCD_01, LCD_02,LCD_03, LCD_04, LCD_05);
SoftwareSerial seGPS(GPS_RX, GPS_TX);
TinyGPS gps;

// vars
int btn;
time_t prevDisplay = 0;

// prototype
void setSerial();
void setLCD();
void lcdText();
void Counter();
int btnKey();
void runGPS();

// functions
void setSerial(/* arguments */) {
  /* setSerial */
  Serial.begin(HARD_SPEED);
  while(!Serial);
  seGPS.begin(SOFT_SPEED);
}

void setLCD(/* arguments */) {
  /* setLCD */
  lcd.begin(LCD_COL, LCD_LIN);
  lcd.clear();
}

void lcdText(String Text, int X = 0, int Y = 0, bool B = false, bool C = false) {
  /* lcdText */
  if( C == true) {
    lcd.clear();
  }

  lcd.setCursor(X, Y);
  lcd.print(Text);

  if(B == true) {
    lcd.blink();
  }
}

void Counter(int X = 0, int Y = 0) {
  /* Counter */
  for (size_t i = 0; i < 99; i++) {
    lcd.setCursor(X, Y);
    lcd.print(i);
    lcd.setCursor(X+2, Y);
    lcd.print("%");
    delay(50);
  }
  lcd.clear();
}

int btnKey() {
  /* btnKey */
  btn = analogRead(BTN);

  if(btn > 1000) return BTN_NONE;
  if(btn > 50) return BTN_RIGHT;
  if(btn > 250) return BTN_UP;
  if(btn > 450) return BTN_DOWN;
  if(btn > 650) return BTN_LEFT;
  if(btn > 850) return BTN_SELECT;

  return BTN_NONE;
}

void runGPS(/* arguments */) {
  /* runGPS */
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

for (unsigned long start = millis(); millis() - start < 1000;) {
    while(seGPS.available()) {
      char c = seGPS.read();
      if(gps.encode(c)) {
        newData = true;
      }
    }
    if (newData) {
      float flat, flon;
      unsigned long age;
      gps.f_get_position(&flat, &flon, &age);

      // lcd
      // latitude
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("W");
      lcd.setCursor(1, 0);
      lcd.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 4);

      // longitude
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("N");
      lcd.setCursor(1, 1);
      lcd.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 4);

      // satellites
      lcd.setCursor(10, 0);
      lcd.print("S:");
      lcd.setCursor(12, 0);
      lcd.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
      //lcd.print( gps.speed() == TinyGPS::GPS_INVALID_SPEED ? 0 : gps.speed());

      // fixes satellites
      lcd.setCursor(10, 1);
      lcd.print("H:");
      lcd.setCursor(12, 1);
      lcd.print(gps.altitude() == TinyGPS::GPS_INVALID_ALTITUDE ? 0 : gps.altitude() / 100);
    }
    gps.stats(&chars, &sentences, &failed);

    // serial debug
    Serial.print(" CHARS: ");
    Serial.print(chars);
    Serial.print(" SENTENCES: ");
    Serial.print(sentences);
    Serial.print(" CSUM ERR: ");
    Serial.println(failed);

    if (chars == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Err: NO GPS DATA");
      lcd.setCursor(0, 1);
      lcd.print("***CHECK WIRING!");
    }
  }
  delay(D1S);
}
