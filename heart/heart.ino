// sda = 21, scl = 22
// red = GND, black = 5V, yellow = 3.3V, white = GND, brown = 18, puple = 19, blue = 21, green = 22,

#include <Adafruit_MPL3115A2.h>
Adafruit_MPL3115A2 baro;
int initial;  //Callibration values
int threshhold;
int beatLimit = 3.5;  //How much of an fraction of the Maximum is detected as a squeeze
int Maxsqueeze = 30;  // Max difference in pressure resulted from a squeeze. Determines threshhold as
int beats;            // How often massage has been done
int beatsSaved = 8;   // How often to compress until heartSaved
bool CPRisPerformed;  // Global value for CPR functionality

#include <FastLED.h>
#define LED_PIN 18      // Define the pin connected to the LED strip (change this to your pin)
#define NUM_LEDS 10     // Define the number of LEDs in your strip
#define BRIGHTNESS 100  // Set the initial brightness (0-255)
CRGB leds[NUM_LEDS];
int fadeSpeed = 5;     //Delay for red flashing

int detectionPin = 33;  // Pin to receive puzzle solve
int cprPin = 17;        // Pin to send compression, white
int statePin = 16;      // Pin to transmit state progression, black

bool heartSaved;        // Is the heart saved?
bool activate = true;  // Immediately shuts heart down, for testing/prototyping, add to line 72, 91 and 106


void setup() {
  Serial.begin(115200);
  pinMode(detectionPin, INPUT_PULLDOWN);
  pinMode(cprPin, OUTPUT);
  pinMode(statePin, OUTPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);  // Change the LED type if necessary
  FastLED.setBrightness(BRIGHTNESS);
  // Initialize all LEDs to off
  FastLED.clear();
  FastLED.show();

  //Keep in or barometre library malfunctions
  while (!Serial)
    ;
  Serial.println("Adafruit_MPL3115A2 test!");

  if (!baro.begin()) {
    Serial.println("Could not find sensor. Check wiring.");
    while (1)
      ;
  }
  initial = baro.getPressure();  //Calllibrate pressure
  threshhold = initial + Maxsqueeze / 3;

  //Reset the raspberry pi sound (heart must be completed!)
  digitalWrite(statePin, HIGH);
  delay(2000);
  digitalWrite(statePin, LOW);
  delay(2000);
}

// Function regulating CPR
void reanimation(float pressure) {
  if (pressure >= threshhold and !CPRisPerformed) {
    Serial.println("Squeeze detected!");
    CPRisPerformed = true;
    digitalWrite(cprPin, HIGH);
  }
  if (pressure <= threshhold and CPRisPerformed) {
    Serial.println("Release detected!");
    CPRisPerformed = false;
    digitalWrite(cprPin, LOW);
    if (digitalRead(detectionPin) == HIGH) {
      beats += 1;
    }
  }
}

void loop() {
  // Serial.println(digitalRead(detectionPin));

  float pressure = baro.getPressure();
  int pulseInput = pressure - initial;  // Relative value for pressure applied
  if (pulseInput < 0) {
    pulseInput = 0;
  }

  //Checks if player is performing CPR
  reanimation(pressure);

  //Initial blinking stare
  if (digitalRead(detectionPin) == LOW) {
    digitalWrite(statePin, LOW);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);  // Low brightness
    }
    FastLED.show();
    delay(50);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(255, 0, 0);  // Low brightness
      FastLED.show();
    }
  }

  //Cpr now possible
  if (beats <= beatsSaved) {
    if (digitalRead(detectionPin) == HIGH) {
      digitalWrite(statePin, HIGH);
      int brightnessValue = map(pulseInput, 0, Maxsqueeze, 0, 255);  // Maps the pressure sensor value to the brightness range
      for (int i = 0; i < NUM_LEDS; i++) {                           // Sets the LED strip brightness
        leds[i] = CRGB(brightnessValue, 0, 0);                       // Set LEDs
      }
      FastLED.show();
    }
  }

  // Heart saved
  if (beats > beatsSaved) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(255, 0, 0);  //Set leds to bright red
    }
    FastLED.show();
    digitalWrite(statePin, LOW);
  }
  delay(50);
}



//------------------------------
// Serial.print("pressure = ");
// Serial.print(pressure);
// Serial.println(" hPa");
// Serial.print(initial);
// Serial.print(",");
// Serial.println(threshhold);


// get the status of Transmitted packet (setup)
// esp_now_register_send_cb(OnDataSent);

// // callback when data is sent
// void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   // Serial.print("\r\nLast Packet Send Status:\t");
//   // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
// }

// #include <esp_now.h>
// #include <WiFi.h>
// uint8_t broadcastAddress[] = { 0x08, 0x3A, 0xF2, 0xB7, 0x36, 0xC0 };
// // Must match the receiver structure
// typedef struct struct_message {
//   int a;
// } struct_message;
// struct_message myData;
// esp_now_peer_info_t peerInfo;

// // Set device as a Wi-Fi Station
// WiFi.mode(WIFI_STA);
// // Init ESP-NOW
// if (esp_now_init() != ESP_OK) {
//   Serial.println("Error initializing ESP-NOW");
//   return;
// }
// // Register peer
// memcpy(peerInfo.peer_addr, broadcastAddress, 6);
// peerInfo.channel = 0;
// peerInfo.encrypt = false;
// // Add peer
// if (esp_now_add_peer(&peerInfo) != ESP_OK) {
//   Serial.println("Failed to add peer");
//   return;
// }

//Natural heartbeat
void fadeRedEffect() {
  // Gradually turn LEDs bright red
  for (int brightness = 0; brightness <= 255; brightness++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(brightness, 0, 0);  // Set LEDs to red with increasing brightness
    }
    FastLED.show();
    delay(fadeSpeed);  // Adjust the delay for the fade speed
  }
  // Gradually fade out the red color
  for (int brightness = 255; brightness >= 0; brightness--) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(brightness, 0, 0);  // Set LEDs to red with decreasing brightness
    }
    FastLED.show();
    delay(fadeSpeed);  // Adjust the delay for the fade speed
  }
}
