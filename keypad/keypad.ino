#include <Keypad.h>
#include <Arduino.h>
#include <Wire.h>
#include "Waveshare_LCD1602_RGB.h"

Waveshare_LCD1602_RGB lcd(16, 2);  //16 characters and 2 lines of show for LCD screen
int r, g, b = 0;                   //Initialise colours

int transmitPinOLED = 26;  // Pin used for transmitting data to OLED
int transmitPinLED = 27;   // Pin used for transmitting data to LED strip
int transmitPinPI = 25;    // Pin used for transmitting to Raspberry PI
// int receiveEndPin = 14;    // Pin used to receive succesful reanimation, currently not in use

const int ROW_NUM = 4;     // Four rows for keypad
const int COLUMN_NUM = 3;  // Three columns
char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte pin_rows[ROW_NUM] = { 0, 18, 5, 16 };   //connect to the row pinouts of the keypad, 2, 7, 6, 4
byte pin_column[COLUMN_NUM] = { 4, 2, 17 };  //connect to the column pinouts of the keypad, 3, 1, 5
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
const String password1 = "19";  //Different stages of total password, Gas austaus Heizung, Jahr wenn gaskonsum verdoppelt, autos in Karikatur
const String password2 = "2050";
const String password3 = "1";
int passwordStage = 1;         //State machine for password
String currentPassword = "0";  //Current sought password

bool foundPassword;
String input;

void setup() {
  Serial.begin(115200);
  pinMode(transmitPinLED, OUTPUT);
  pinMode(transmitPinOLED, OUTPUT);
  pinMode(transmitPinPI, OUTPUT);
  // pinMode(receiveEndPin, INPUT_PULLDOWN);
  digitalWrite(transmitPinLED, LOW);  // Reset sending
  digitalWrite(transmitPinOLED, LOW);
  digitalWrite(transmitPinPI, LOW);

  lcd.init();  //Start LCD
}

void loop() {
  //   // digitalWrite(transmitPin, HIGH);
  //   // Serial.print(digitalRead(buttonPin));
  //   // Serial.println(password == input);
  //   // digitalWrite(buttonPower, HIGH);

  char key = keypad.getKey();

  //State of password guessing
  if (passwordStage == 1) {
    lcd.setCursor(0, 0);
    lcd.send_string("Passwort (1)    ");
    currentPassword = password1;
  }
  if (passwordStage == 2) {
    lcd.setCursor(0, 0);
    lcd.send_string("Passwort (2)    ");
    currentPassword = password2;
  }
  if (passwordStage == 3) {
    lcd.setCursor(0, 0);
    lcd.send_string("Passwort (3)    ");
    currentPassword = password3;
  }
  if (passwordStage == 4) {
    // if (digitalRead(receiveEndPin) == LOW) {
      foundPassword = true;
      r = 255;
      g = 0;
      b = 0;
      lcd.setRGB(r, g, b);
      lcd.setCursor(0, 0);
      lcd.send_string("Reanimiere Herz!");
      lcd.setCursor(0, 1);
      lcd.send_string("Reanimate Heart!");
    // } else {
      // if (digitalRead(receiveEndPin) == HIGH) {
        // foundPassword = true;
        // r = 40;
        // g = 255;
        // b = 40;
        // lcd.setRGB(r, g, b);
        // lcd.setCursor(0, 0);
        // lcd.send_string("Herz gerettet");
        // lcd.setCursor(0, 1);
        // lcd.send_string("Heart saved");
      // }
    // }
  }

  if (key and !foundPassword) {
    Serial.println(key);
    r = 255;  //Set background standard to blue
    g = 255;
    b = 255;
    lcd.setRGB(r, g, b);

    if (key == '*') {
      input = "";  // clear input password
      lcd.setCursor(0, 1);
      lcd.send_string("                ");  //Text cleared

    } else if (key == '#') {
      if (currentPassword == input) {
        r = 40;
        g = 255;
        b = 40;
        lcd.setRGB(r, g, b);
        lcd.setCursor(0, 0);
        lcd.send_string("                ");  //Text cleared
        lcd.setCursor(0, 0);
        lcd.send_string("Passwort ->   ");  //Right answer, background green
        lcd.setCursor(0, 1);
        lcd.send_string("                ");  //Text cleared
        lcd.setCursor(0, 1);
        lcd.send_string("Korrekt");

        passwordStage += 1;
        input = "";   // clear input password
        delay(3000);  //One second delay to see arrow effect

      } else {
        Serial.println("password is incorrect");
        lcd.setCursor(0, 1);
        lcd.send_string("                ");  //Text cleared
        lcd.setCursor(0, 1);
        lcd.send_string("Falsch");  //Wrong answer, background red
        r = 255;
        g = 0;
        b = 0;
        lcd.setRGB(r, g, b);
        input = "";  // clear input password
      }

    } else {
      input += key;  // append new character to input password string
      lcd.setCursor(0, 1);
      lcd.send_string("                ");  //Text cleared
      lcd.setCursor(0, 1);
      lcd.send_string(input.c_str());
    }
  }

  if (foundPassword) {
    digitalWrite(transmitPinLED, HIGH);  // Send signals that keypad puzzle is solved
    digitalWrite(transmitPinOLED, HIGH);
    digitalWrite(transmitPinPI, HIGH);

    // Serial.println("transmitting signal");
  }

  delay(10);
}


----------------
//Spare code if one long password is desired againb:


// // Check if the main password contains the sub-passwords
// int wrongGuesses = 3;
// if (input.indexOf(password1) != -1) {
//   wrongGuesses -= 1;
// }
// if (input.indexOf(password2) != -1) {
//   wrongGuesses -= 1;
// }
// if (input.indexOf(password3) != -1) {
//   wrongGuesses -= 1;
// }
// lcd.setCursor(0, 1);
// lcd.send_string("                ");  //Text cleared
// //Indicate how many were wrong
// if (wrongGuesses == 3) {
//   lcd.setCursor(0, 1);
//   lcd.send_string("3 falsch");
// }
// if (wrongGuesses == 2) {
//   lcd.setCursor(0, 1);
//   lcd.send_string("2 falsch");
// }
// if (wrongGuesses == 1) {
//   lcd.setCursor(0, 1);
//   lcd.send_string("1 falsch");
// }
// if (wrongGuesses == 0) {
//   // Check if the main password contains only the specified sub-passwords
//   if (input.length() == password.length()) {
//     lcd.setCursor(0, 1);
//     lcd.send_string("Reihenfolge");
//   } else {
//     lcd.setCursor(0, 1);
//     lcd.send_string("Tippfehler");
//   }
// }
