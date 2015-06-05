
#include <Adafruit_NeoPixel.h>

#define CELL_COUNT 20
#define PIN 6
#define DELAY 1000
#define FADE_SPEED 250
#define COLORCHANGE_SPEED 30
#define COLOR_SPECTRUM 255
#define BRIGHTNESS 127

//#define SERIAL_COMMUNICATION

Adafruit_NeoPixel strip = Adafruit_NeoPixel(CELL_COUNT, PIN, NEO_GRB + NEO_KHZ800);
uint32_t on;
uint32_t off;

bool cells[CELL_COUNT];
bool cellsBuffer[CELL_COUNT];
float brightness[CELL_COUNT];

long next_update;
long last_tick;
long current_tick;
long next_color_rotation;

bool rules[8] = {false, false, false, true, true, true, true, false};

void setup() {
  // initialize serial communication
  Serial.begin(9600);

  // clear strip
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);


  on = strip.Color(255, 255, 255);
  off = strip.Color(0, 0, 0);

  // initialize cells to random states
  for (byte i = 0; i < CELL_COUNT; i++) {
    if (random(2) > 0) {
      cells[i] = true;
      brightness[i] = 255;
    } else {
      cells[i] = false;
      brightness[i] = 0;
    }
  }

  current_tick = millis();
  next_update = current_tick + DELAY;
  next_color_rotation = current_tick + COLORCHANGE_SPEED;
}

void loop() {
  last_tick = current_tick;
  current_tick = millis();

  if (current_tick > next_update) {
    // generate evolution
    generate();

#ifdef SERIAL_COMMUNICATION
    // send state to computer
    for (byte i = 0; i < CELL_COUNT; i++) {
      Serial.print(cells[i]);
    }
    Serial.print('\n');
#endif

    next_update += DELAY;
  }

  if (current_tick > next_color_rotation + COLORCHANGE_SPEED * 1000) next_color_rotation += COLORCHANGE_SPEED * 1000;

  displayCells(current_tick - last_tick);
}

void generate() {
  // copy cells into buffer
  for (byte i = 0; i < CELL_COUNT; i++) {
    cellsBuffer[i] = cells[i];
  }

  // perform math
  bool l, c, r;
  // set left (offscreen) value to random state
  if (random(2) > 0) {
    l = true;
  } else {
    l = false;
  }

  for (byte i = 0; i < CELL_COUNT; i++) {
    c = cells[i];

    // if we have hit the rightmost bound set the right (offscreen) value to random state
    if (i + 1 >= CELL_COUNT) {
      if (random(2) > 0) {
        cells[i] = true;
      } else {
        cells[i] = false;
      }
    } else {
      r = cells[i + 1];
    }

    // Implementing the Wolfram rules
    if (l == true && c == true && r == true) {
      cells[i] = rules[0];
    }
    if (l == true && c == true && r == false) {
      cells[i] = rules[1];
    }
    if (l == true && c == false && r == true) {
      cells[i] = rules[2];
    }
    if (l == true && c == false && r == false) {
      cells[i] = rules[3];
    }
    if (l == false && c == true && r == true) {
      cells[i] = rules[4];
    }
    if (l == false && c == true && r == false) {
      cells[i] = rules[5];
    }
    if (l == false && c == false && r == true) {
      cells[i] = rules[6];
    }
    if (l == false && c == false && r == false) {
      cells[i] = rules[7];
    }

    l = c;
  }
}

void displayCells(long tick) {
  for (byte i = 0; i < CELL_COUNT; i++) {
    if (cells[i]) {
      if (brightness[i] < 255) brightness[i] += map(tick, 0, FADE_SPEED, 0, 255);
      if (brightness[i] > 255) brightness[i] = 255;
    } else {
      if (brightness[i] > 0) brightness[i] -= map(tick, 0, FADE_SPEED, 0, 255);
      if (brightness[i] < 0) brightness[i] = 0;
    }
    strip.setPixelColor(i, generateColor(i, brightness[i]));
  }

  strip.show();
}

uint32_t generateColor(byte i, byte brightness) {
  byte wheelPos = map(current_tick + next_color_rotation - COLORCHANGE_SPEED * 1000, 0, COLORCHANGE_SPEED * 1000, 0, 255);

  wheelPos = 255 - wheelPos;
  wheelPos = (int)(wheelPos + ((float)i / CELL_COUNT * COLOR_SPECTRUM)) % 256;
  byte r, g, b;
  if (wheelPos < 85) {
    r = 255 - wheelPos * 3;
    g = 0;
    b = wheelPos * 3;
  } else if (wheelPos < 170) {
    wheelPos -= 85;
    r = 0;
    g = wheelPos * 3;
    b = 255 - wheelPos * 3;
  } else {
    wheelPos -= 170;
    r = wheelPos * 3;
    g = 255 - wheelPos * 3;
    b = 0;
  }
  float br = brightness / 255.0f;
  return strip.Color(r * br, g * br, b * br);
}
