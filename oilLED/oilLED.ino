#include <FastLED.h>

#define LED_PIN 2                // Define the pin connected to the LED strip
#define NUM_LEDS 120             // Define the number of LEDs in the strip
#define LED_TYPE WS2812B         // Define the LED strip type
#define COLOR_ORDER GRB          // Define the color order (RGB, GRB, etc.)
#define NUM_LIGHTS 6             // Number of white lights (each with 3 LEDs)
CRGB leds[NUM_LEDS];             // Initialize an array for storing LED colors
int lightPositions[NUM_LIGHTS];  // Array to store positions of each light
int brightnessValue = 100;       // Adjust the brightness (0-255)
int moveDelayValue = 50;         // Adjust the movement delay (milliseconds)

int ventilPin = 6;   // Turns decorative fan on/off
int receivePin = 5;  //Pin to receive shutdown message

void setup() {
  pinMode(receivePin, INPUT);
  pinMode(ventilPin, OUTPUT);
  pinMode(grnd, INPUT);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  fill_solid(leds, NUM_LEDS, CRGB::Black);  // Turn off all LEDs initially
  FastLED.setBrightness(brightnessValue);   // Set initial brightness
  FastLED.show();
  // Initialize positions for each light
  for (int i = 0; i < NUM_LIGHTS; i++) {
    lightPositions[i] = i * (NUM_LEDS / NUM_LIGHTS);  // Equally distribute lights across the strip
  }
}

void loop() {
  // Turn all of LED strip off
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  //Shut LEDs down when puzzle solved, otherwise set to default
  if (digitalRead(receivePin) == HIGH and brightnessValue != 0) {
    brightnessValue -= 2;
    FastLED.setBrightness(brightnessValue);  // Update brightness
    moveDelayValue += 5;
    digitalWrite(ventilPin, LOW);
  }
  if (digitalRead(receivePin) == LOW) {
    digitalWrite(ventilPin, HIGH);
    moveDelayValue = 50;
    brightnessValue = 100;
  }

  // Update positions for each light
  for (int i = 0; i < NUM_LIGHTS; i++) {
    // Move each light to the next position
    lightPositions[i] = (lightPositions[i] + 1) % NUM_LEDS;

    // Set the LEDs for the current light
    for (int j = 0; j < 3; j++) {
      int ledIndex = (lightPositions[i] + j) % NUM_LEDS;
      leds[ledIndex] = CRGB::White;
    }
  }

  FastLED.show();
  delay(moveDelayValue);
}
