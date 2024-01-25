#include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(5, 4, U8X8_PIN_NONE);  // Pins for oled  5 = brown, 4 = purple
long showProduction = 40;                                      // How much oil is extracted
int showRate = 12;                                            // At what rate oil is extracted
int receivePin = 8;                                           // Receives whether the factory is active or not
bool reset;

void setup() {
  Serial.begin(115200);
  pinMode(receivePin, INPUT);

  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_pxplusibmcgathin_f);
}

void loop() {
  int showRateDecimal = random(0, 9);  //Random Decimal for rate
  if (digitalRead(receivePin) == LOW) {
    if (reset){      //Ensure no manual restart is required
      showProduction = 40;
      reset = 0;
    }
    showProduction += showRate / 5;
    showRate += random(-1, 2);  //Random shift of 1
    if (showRate < 10) {
      showRate = 10;
    }
    if (showRate > 20) {
      showRate = 20;
    }
    u8x8.drawString(0, 7, "Status: AKTIV  ");
  } else {
    reset = 1;
    u8x8.drawString(0, 7, "Status: INAKTIV");
    if (showRate == 0) {
      showRateDecimal = 0;
    } else {
      showRate -= 1;
    }
  }

  //Update production and rate values displayed
  u8x8.setCursor(0, 0);
  u8x8.print("Olproduktion:");
  u8x8.setCursor(0, 1);
  u8x8.print(showProduction);
  u8x8.print(" Tonnen");
  u8x8.setCursor(0, 3);
  u8x8.print("Produktionsrate:");
  u8x8.setCursor(0, 4);
  u8x8.print(showRate);
  u8x8.print(",");
  u8x8.print(showRateDecimal);
  u8x8.print("%");
}
