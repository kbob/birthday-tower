#ifndef SIGMAP_included
#define SIGMAP_included

#include <math.h>

// Generalized sigmoid mapping function.  Scales its input X, applies
// a logistic function, then scales its output, Y.
//
// Logistic function logistic(x) = 1 / (1 + e^(-x))
//
//    X is linearly scaled: X' = slope_x * (X - center_x).
//
//    Y = logistic(X').   0 < Y < +1.
//
//    Y' = (max_y - min_y) * Y + min_y.  min_y <= Y' <= max_y.
//
// So the four coefficients are slopeX, centerX, minY, maxY.
//
// By default, slopeX = 1, centerX = 0 (identity transform on input),
//             minY = 0, maxY = 1 (0 < Y' < 1)
// giving the standard logistic function.

struct Sigmap {

  Sigmap()
  : slope_x(1.0f),
    center_x(0.0f),
    max_y(1.0),
    min_y(0.0)
  {}

  float map(float x) {
    float xx = slope_x * (x - center_x);
    float y = 1.0f / (1.0f + expf(-xx));
    float yy = min_y + (max_y - min_y) * y;
    return yy;
  }

  float slope_x;
  float center_x;
  float max_y;
  float min_y;
};

#endif /* !SIGMAP_included */
