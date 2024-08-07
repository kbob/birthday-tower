#ifndef AUDIO_MANAGER_included
#define AUDIO_MANAGER_included

class AudioManager {

public:
  // volume, frequency, and duration are [0..1].
  static void
  trigger_launch(float frequency, float duration);
  static void
  trigger_top_bounce(float volume, float frequency, float duration);
  static void
  trigger_bottom_bounce(float volume, float frequeny, float duration);

  static void setup();
  static void loop();
};

#endif /* !AUDIO_MANAGER_included */
