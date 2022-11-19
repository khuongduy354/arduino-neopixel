// #include <Adafruit_NeoPixel.h>
#include "FastLED.h"
#define NUM_LEDS 16
#define LED_INPUT_PIN 3
#define LED_TYPE WS2812
#define BRIGHTNESS 100

CRGB leds[NUM_LEDS];
// how to design leds
// effects: stacking
// state: rainbow, dot moving 1 side, dot moving 2 side, scanner
// final combo: https://www.youtube.com/watch?v=seG5tip6wBw

enum direction { FORWARD, REVERSE };
enum pattern {
  SWEEPER,
  RAINBOW_CYCLE,
  THEATER_CHASE,
  COLOR_WIPE,
  SCANNER,
  FADE
};

//============ HELPERS ==========>
CRGB DimColor(CRGB color) {
  return CRGB(color.red * 3 / 4, color.green * 3 / 4, color.blue * 3 / 4);
}
CRGB Wheel(int WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

class Pattern {
  int interval;
  pattern type;
  int lastTime; // last millis
  int current_pos;
  direction dir;

public:
  Pattern(int _interval, direction _dir, pattern _type) {
    interval = _interval;
    lastTime = 0;
    dir = _dir;
    type = _type;
    current_pos = _dir == FORWARD ? 0 : NUM_LEDS;
  }

  void Update() {
    if ((millis() - lastTime) > interval) {
      lastTime = millis();
      switch (type) {
      case SWEEPER:
        SweeperUpdate();
        break;
      case SCANNER:
        ScannerUpdate();
        break;
      case RAINBOW_CYCLE:
        RainbowUpdate();
        break;
      default:
        break;
      };
    }
  }
  void RainbowUpdate() {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = Wheel(((i * 256 / NUM_LEDS) + current_pos) & 255);
    }
    FastLED.show();
  }
  void SweeperUpdate() {
    int addition = dir == FORWARD ? 1 : -1;
    if (current_pos > NUM_LEDS) {
      current_pos = 0;
    } else if (current_pos < 0) {
      current_pos = NUM_LEDS;
    }

    leds[current_pos] += CRGB::Blue;
    leds[current_pos - addition] = CRGB::Black;
    current_pos += addition;

    FastLED.show();
  }
  void ScannerUpdate() {
    int addition = dir == FORWARD ? 1 : -1;
    leds[current_pos] += CRGB::Yellow;
    leds[current_pos - addition] = DimColor(CRGB::Yellow);
    current_pos += addition;

    FastLED.show();
  }
  void move_on() {}
};

// Patern directions supported:
class NeoPixelLed {
public:
  Pattern *patterns[50];
  int current_active_pattern;
  int num_patterns;
  NeoPixelLed() {
    current_active_pattern = 0;
    num_patterns = 0;
  }

  void Update() {
    for (int i = current_active_pattern; i < num_patterns; i++) {
      patterns[i]->Update();
    }
  }
  void addPattern(Pattern *p, bool isStack = false) {
    patterns[num_patterns] = p;
    current_active_pattern = isStack ? current_active_pattern : num_patterns;
    num_patterns++;
  }
};

NeoPixelLed led1;

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, LED_INPUT_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // led1.addPattern(new Pattern(500, FORWARD, SWEEPER));
  led1.addPattern(new Pattern(500, REVERSE, SCANNER), true);
  led1.addPattern(new Pattern(500, REVERSE, RAINBOW_CYCLE), true);
}

void loop() { led1.Update(); }
