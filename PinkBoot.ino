// Adafruit NeoPixel library.
#include <Adafruit_NeoPixel.h>

const uint8_t gamma[] PROGMEM = { // Gamma correction table for LED brightness
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

// NeoPixel Data Output Pin.
#define STRIP_PIN 12
// Number of NeoPixels
#define STRIP_PIX_MAX 40

// Init NeoPixel strip objects.
// Parameter 1 = number of pixels in strip.
// Parameter 2 = Arduino pin number (most are valid).
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs).
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers).
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products).
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2).
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_PIX_MAX, STRIP_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk:
//            Add a 1000 uF capacitor across pixel power leads.
//            Add a 300 - 500 Ohm resistor on first pixel's data input pin
//            and minimize distance between Arduino and first pixel.
//            Avoid connecting on a live circuit...if you must, connect GND first.

// EEPROM library.
#include <EEPROM.h>
// Persistent Shoe State.
byte shoeState = 0;
// Number of Shoe States.
#define SHOE_STATE_MAX 3 

// setup() runs once at reset.
void setup() {
  // Init Strip.
  strip.begin();
  // Refresh Strip.
  strip.clear();
  strip.show();
  
  // EEPROM Init Code Base Address
  #define EEPROM_INIT_CODE_ADDR 0x00
  // Check EEPROM Init Code
  byte shoeStateInit[] = {0xde, 0xad, 0xbe, 0xaf, 0xaa};
  for(int b = EEPROM_INIT_CODE_ADDR; b < EEPROM_INIT_CODE_ADDR + sizeof(shoeStateInit); b++) {
    // EEPROM = Init Code?
    if(EEPROM.read(b) != shoeStateInit[b]) {
      // No, Write Init Code to EEPROM.
      EEPROM.write(b, shoeStateInit[b]);
      // Set Not Initialized Flag.
      shoeState = -1;
    }
  }
  // EEPROM Init Code?
  if(shoeState == 0) {
    // Yes, Read Shoe State from EEPROM.
    shoeState = EEPROM.read(sizeof(shoeStateInit));
  }
  // Increment Shoe State.
  shoeState++;
  if(shoeState >= SHOE_STATE_MAX) {
    shoeState = 0;
  }
  // Write Shoe State to EEPROM.
  EEPROM.write(sizeof(shoeStateInit), shoeState);
}

// loop() runs continuously after setup().
void loop() {
  switch(shoeState) {
    case 0: {
      // Rainbow Cycle. Brightness 75%. Speed 50 millis().
      strip.setBrightness(0x3f);
      #define RAINBOW_CYCLE_DLY 50
      while(true) {
        // Rainbow Color Offset.
        for(int rainbow = 0; rainbow < 256; rainbow+= 4) {
          // Set Pixels Color.
          for(int p = 0; p < strip.numPixels(); p++) {
            // Pixel Color = Rainbow + Pixel Offsets.
            strip.setPixelColor(p, wheelStrip(rainbow));
          }
          // Refresh Strip.
          strip.show();
          delay(RAINBOW_CYCLE_DLY);
        }
      }
    }
    case 1: {
      // Rainbow Wipe. Brightness 75%. Speed 50 millis().
      strip.setBrightness(0x3f);
      #define RAINBOW_WIPE_DLY 50
      while(true) {
        // Rainbow Color Offset.
        for(int rainbow = 0; rainbow < 256; rainbow+= 8) {
          // Set Pixels Color.
          for(int p = 0; p < strip.numPixels(); p++) {
            // Pixel Color = Rainbow + Pixel Offsets.
            strip.setPixelColor(p, wheelStrip((rainbow + p * 8) & 0xff));
          }
          // Refresh Strips.
          strip.show();
          delay(RAINBOW_WIPE_DLY);
        }
      }
    }
  }
}  

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t wheelStrip(byte wheelPos) {
  wheelPos = 255 - wheelPos;
  if(wheelPos < 85) {
    return strip.Color(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  else if(wheelPos < 170) {
    wheelPos -= 85;
    return strip.Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  else {
    wheelPos -= 170;
    return strip.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
  }
}

