// Pin Assignments
//
// Audio I2S
//   23 MCLK
//   21 BCLK1
//   20 LRCLK
//    7 OUT1A
// Audio I2C
//   19 SCL
//   18 SDA
// Audio Volume
//   15 VOL
// Audio SPI
//   13 SCK
//   12 MISO
//   11 MOSI
//    6 MEMCS
// Audio Unused
//    8 Audio Data In
//   10 SD card chip select
// LED Strip
//   1 WS2812 DATA
// Arcade Button
//   2 BUTTON
// Arcade Button LED
//   3 BUTTON_LED

#include <Bounce2.h>
#include <WS2812Serial.h>

#include "AudioManager.h"
#include "GoldenColors.h"

#define LED_STRIP_PIN 1
#define BUTTON_PIN 2
#define BUTTON_LED_PIN 3

#define BUTTON_INTERVAL_MSEC 5
#define LED_COUNT 200
#define LED_FORMAT WS2812_GRB

#define IDLE_TIMEOUT_MSEC 120000        // start "attract mode" after idle time

//// // /   /   /   /   /   /   /   /   /   /   /   /   /   /   /   /   /
/// LEDs
//
byte LED_drawing_memory[3 * LED_COUNT];
DMAMEM byte LED_display_memory[12 * LED_COUNT];
WS2812Serial leds(
  LED_COUNT,
  LED_display_memory,
  LED_drawing_memory,
  LED_STRIP_PIN,
  LED_FORMAT
);

uint32_t get_pixel_color(uint16_t index) {
  if (LED_FORMAT < 6) {
    // RGB
    byte *p = &LED_drawing_memory[3 * index];
    return p[0] | p[1] << 8 | p[2] << 16;
  } else {
    // RGBW
    byte *p = &LED_drawing_memory[4 * index];
      return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
  }
}

void set_pixel_color(uint16_t index, uint32_t color) {
  if (index >= LED_COUNT) {
    pinMode(BUTTON_LED_PIN, OUTPUT);
    while (1) digitalWrite(BUTTON_LED_PIN, millis() % 1000 < 500);
  }
  leds.setPixelColor(index, color);
}

// WS2812Serial is missing its implementation for busy().  There's been
// a pull request outstanding since 2019, so I don't think Paul is going
// to add it.
//
// We abuse the definition here slightly -- when this busy returns false,
// the drawing buffer is available, but the LEDs may still be in their
// 300 microsecond cooldown period.
bool WS2812Serial::busy()
{
#if defined(KINETISK)
  return DMA_ERQ & (1 << dma->channel);
#elif defined(KINETISL)
  return dma->CFG_DCF & DMA_DCR_ERQ;
#elif defined(__IMXRT1062__)
  return DMA_ERQ & (1 << dma->channel);
#endif
}


//// // // //   /   /   /   /   /   /   /   /   /   /   /   /   /   /   /
/// Sparkler
//
class Sparkler {

  enum {
    MAX_SPARKS = 200,
    MAX_VEL = 100 * 65536,    // steps/tick * 2**16 for a 1 msec pulse
    CONTRAIL_LENGTH = 8,      // length of a 1 step/tick spark's contrail
    CONTRAIL_MIN = 5,         // brightness of a contrail's trailing pixel
    SPARKBURST_MIN = 5,
    SPARKBURST_MAX = 30,
    SPARKBURST_SLOWEST = 200, // msec button duration
    SPARKBURST_FASTEST = 40,
  };

  struct spark {
    uint32_t pos;             // fixed point 16.16 unsigned
    int32_t vel;              // fixed point 16.16 signed
    uint32_t color;
    float volume;
    uint32_t contrail_pixels;
    float contrail_decay;
  };

public:
  Sparkler(uint32_t pixel_count)
  : pixel_count(pixel_count),
    spark_count(0),
    color_index(0)
  {}

  bool is_idle() const { return spark_count == 0; }

  // create a new spark
  void ignite(uint32_t msec, bool has_sound = true) {
    uint32_t pos = 0;
    int32_t vel = MAX_VEL / msec;
    uint32_t color = golden_color_RGB(color_index++, 65535, 65535);
    uint32_t contrail_pixels = max(1, CONTRAIL_LENGTH * vel / 65536);
    float contrail_decay = powf(CONTRAIL_MIN / 255.0f, 1.0f / (contrail_pixels - 1));
    // Serial.printf("contrail pixels = %d, decay = %g\n", contrail_pixels, contrail_decay);

    if (spark_count < MAX_SPARKS) {
      spark *sp = &sparks[spark_count++];
      sp->pos = pos;
      sp->vel = vel;
      sp->color = color;
      sp->volume = has_sound ? 1.0 : 0.0;
      sp->contrail_pixels = contrail_pixels;
      sp->contrail_decay = contrail_decay;
    }
    if (has_sound) {
      AudioManager::trigger_launch(vel);
    }
  }

  // update all the LEDs
  void light() {
    for (size_t i = 0; i < spark_count; i++) {
      const spark *sp = &sparks[i];
      uint32_t index = sp->pos >> 16;
      uint32_t c0 = sp->color;
      uint8_t r0 = c0 >> 16 & 0xFF, g0 = c0 >> 8 & 0xFF, b0 = c0 >> 0 & 0xFF;

      // Leave a fading contrail behind each spark.
      int dir = sp->vel > 0 ? -1 : +1;
      int n_pix = sp->contrail_pixels;
      float decay = sp->contrail_decay;
      for (int j = 0; j < n_pix; j++) {  // N.B. j must be signed
        int j_index = index + dir * j;
        if (j_index < 0 || (uint32_t)j_index >= pixel_count)
          break;
        uint32_t c1 = get_pixel_color(j_index);
        uint8_t r = min(255u, r0 + (c1 >> 16 & 0xFF));
        uint8_t g = min(255u, g0 + (c1 >>  8 & 0xFF));
        uint8_t b = min(255u, b0 + (c1 >>  0 & 0xFF));
        set_pixel_color(j_index, (r << 16 | g << 8 | b << 0));
        r0 *= decay; g0 *= decay; b0 *= decay;
      }
    }
  }

  // move the sparks forward
  void advance() {
    for (size_t i = 0; i < spark_count; i++) {
      spark *sp = &sparks[i];
      uint32_t new_pos = sp->pos + sp->vel;
      bool bounced = false;
      if (sp->vel < 0 && (uint32_t) -sp->vel > sp->pos) {
        // Bounce off the low end
        new_pos = ~new_pos + 1;
        bounced = true;
        if (sp->volume) {
          sp->volume /= 1.3;
          AudioManager::trigger_bottom_bounce(sp->volume, -sp->vel);
        }
      } else if (new_pos >= pixel_count << 16) {
        // Bounce off the high end
        new_pos = (2 * pixel_count << 16) - new_pos;
        bounced = true;
        if (sp->volume) {
          sp->volume /= 1.3;
          AudioManager::trigger_top_bounce(sp->volume, sp->vel);
        }
      }
      if (bounced) {
        sp->vel = -sp->vel;
        uint32_t c = sp->color;
        uint8_t r = (c >> 16 & 0xFF) / 3;
        uint8_t g = (c >>  8 & 0xFF) / 3;
        uint8_t b = (c >>  0 & 0xFF) / 3;
        sp->color = r << 16 | g << 8 | b << 0;
      }
      sp->pos = new_pos;
    }

    // Clean up any extinguished sparks.

    for (spark *sp = sparks; sp < sparks + spark_count; ) {
      if (sp->color)
        sp++;
      else {
        *sp = sparks[--spark_count];
      }
    }
  }

  // ignite a bunch of sparks
  void sparkburst() {
    int n_sparks = random(SPARKBURST_MAX - SPARKBURST_MIN) + SPARKBURST_MIN;
    for (int i = 0; i < n_sparks; i++) {
      const uint32_t s = SPARKBURST_SLOWEST;
      const uint32_t f = SPARKBURST_FASTEST;
      uint32_t spark_speed = random(s - f) + f;
      ignite(spark_speed, false);
    }
  }

private:
  uint32_t pixel_count;
  size_t spark_count;
  uint32_t color_index;
  spark sparks[MAX_SPARKS];
};

Sparkler sparkler(LED_COUNT);


//// // // // //   /   /   /   /   /   /   /   /   /   /   /   /   /   /
/// LED Manager
//
class LEDManager {

public:
  static void setup() {
    leds.begin();
  }

  static void loop() {
    if (!leds.busy()) {
      // This gives maxiuum overlap between the hardware DMA and pixel update.
      // Busy returns false as soon as the UART is done.  During the 300 usec
      // dead time required by Neopixels, we can compute and format the next
      // frame.
      leds.clear();
      static uint32_t idle_started_msec = 759201; // random big number
                                                  // to get this show started
      uint32_t now = millis();
      if (idle_started_msec && now - idle_started_msec >= IDLE_TIMEOUT_MSEC) {
        // We've been idle: start "attract mode".
        sparkler.sparkburst();
        idle_started_msec = 0;
      }
      sparkler.light();
      sparkler.advance();
      if (sparkler.is_idle()) {
        if (!idle_started_msec)
        idle_started_msec = now;
      } else {
        idle_started_msec = 0;
      }
      leds.show();
    }
  }
};

//// // // // // //   /   /   /   /   /   /   /   /   /   /   /   /   /
/// Button Manager
//
Button button;

class ButtonManager {

public:
  static void setup() {
    button.attach(BUTTON_PIN, INPUT_PULLUP);
    button.interval(BUTTON_INTERVAL_MSEC);
    button.setPressedState(LOW);
    button.update();
    (void) button.changed(); // Ignore the first one.

    pinMode(BUTTON_LED_PIN, OUTPUT);
    digitalWrite(BUTTON_LED_PIN, LOW);
  }

  static void loop() {
    button.update();
    if (button.changed()) {
      digitalWrite(BUTTON_LED_PIN, button.isPressed() ? HIGH : LOW);
      if (button.released()) {
        uint32_t dur = button.previousDuration();
        sparkler.ignite(dur);
      }
    }
  }
};


//// // // //   /   /   /   /   /   /   /   /   /   /   /   /   /   /   /
/// Arduino
//

void setup() {

  Serial.begin(9600);

  ButtonManager::setup();
  LEDManager::setup();
  AudioManager::setup();
}

void loop() {
  ButtonManager::loop();
  LEDManager::loop();
  AudioManager::loop();
}

//       1         2         3         4         5         6         7         8
// 456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789|
