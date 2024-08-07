#ifndef AUDIO_MANAGER_included
#define AUDIO_MANAGER_included

class AudioManager {

public:

  // Velocity is in 16.16 fixed-point format, representing
  // the number of LEDs moved per refresh.  The refresh rate is
  // roughly 158 Hz.  So velocity of 1000 would be
  // 2.4xxx LEDs per second.  Completely unintuitive, sorry.
  //
  //   1000 / 65536 = 0.0153 LEDs/refresh
  //   0.0153 LEDs/refresh * 158 refresh/sec = 2.42 LEDs/sec
  //
  // Typical "fast" velocities are 100,000 to 150,000 (360-550 LED/sec)
  // "Slow" velocities are anything below 20,000 (48 LEDs/sec)
  // YMMV.
  static void trigger_launch(int velocity);
  static void trigger_top_bounce(float volume, int velocity);
  static void trigger_bottom_bounce(float volume, int velocity);

  static void setup();
  static void loop();
};

#endif /* !AUDIO_MANAGER_included */
