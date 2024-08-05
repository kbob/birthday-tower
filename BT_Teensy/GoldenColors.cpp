#include "GoldenColors.h"

#include <Arduino.h>

uint32_t golden_color_RGB(uint32_t index, uint16_t sat, uint16_t val)
{
  const uint64_t MAGIC = 243019;  // 6 * 65536 * (phi - 1)
  uint64_t x = MAGIC * (uint64_t)index;
  uint32_t h6 = x % (6 * 65536);
  uint32_t h_i = h6 / 65536;
  uint32_t f = h6 - h_i * 65536;
  uint32_t s = sat;
  uint32_t v = val;
  uint32_t p = v * (65535 - s);
  uint32_t q = v * (65535 - f * s / 65535);
  uint32_t t = v * (65535 - (65535 - f) * s / 65535);

  uint16_t red, green, blue;
  switch (h_i) {
  case 0:
    red = v;
    green = t;
    blue = p;
    break;

  case 1:
    red = q;
    green = v;
    blue = p;
    break;

  case 2:
    red = p;
    green = v;
    blue = t;
    break;

  case 3:
    red = p;
    green = q;
    blue = v;
    break;

  case 4:
    red = t;
    green = p;
    blue = v;
    break;

  case 5:
    red = v;
    green = p;
    blue = q;
    break;
  }

  return red / 256 << 16 | green / 256 << 8 | blue / 256 << 0;
}
