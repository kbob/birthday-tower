#include "AudioManager.h"

#include <Audio.h>

#include "AudioSynthMutableBassDrum.h"
#include "AudioSynthMutableFMDrum.h"
#include "AudioEffectMutableReverb.h"

// GUItool: begin automatically generated code
AudioSynthMutableFMDrum  top1;           //xy=75,40
AudioSynthMutableFMDrum  top2;           //xy=75,80
AudioSynthMutableFMDrum  top3;           //xy=75,120
AudioSynthMutableFMDrum  top4;           //xy=75,160

AudioSynthMutableFMDrum  bottom1;        //xy=75,370
AudioSynthMutableFMDrum  bottom2;        //xy=75,410
AudioSynthMutableFMDrum  bottom3;        //xy=75,450
AudioSynthMutableFMDrum  bottom4;        //xy=75,490

AudioSynthMutableBassDrum launch1;       //xy=75,570

AudioMixer4              mixer1;         //xy=235,100
AudioMixer4              mixer2;         //xy=235,430

AudioMixer4              mixer3;         //xy=395,100
AudioEffectDelay         delay1;         //xy=395,200
AudioEffectDelay         delay2;         //xy=395,330
AudioMixer4              mixer4;         //xy=395,430

AudioEffectMutableReverb reverb1;        //xy=555,265

AudioOutputI2S           i2s1;           //xy=715,245
#ifdef AUDIO_INTERFACE
AudioOutputUSB           usb1;           //xy=715,285
#endif

AudioConnection          patchCord1(top1, 0, mixer1, 0);
AudioConnection          patchCord2(top2, 0, mixer1, 1);
AudioConnection          patchCord3(top3, 0, mixer1, 2);
AudioConnection          patchCord4(top4, 0, mixer1, 3);
AudioConnection          patchCord5(bottom1, 0, mixer2, 0);
AudioConnection          patchCord6(bottom2, 0, mixer2, 1);
AudioConnection          patchCord7(bottom3, 0, mixer2, 2);
AudioConnection          patchCord8(bottom4, 0, mixer2, 3);
AudioConnection          patchCord9(launch1, 0, mixer4, 2);
AudioConnection          patchCord10(mixer1, 0, mixer3, 0);
AudioConnection          patchCord11(mixer2, 0, mixer4, 0);
AudioConnection          patchCord12(mixer3, 0, reverb1, 0);
AudioConnection          patchCord13(mixer3, delay1);
AudioConnection          patchCord14(delay1, 0, mixer4, 1);
AudioConnection          patchCord15(delay2, 0, mixer3, 1);
AudioConnection          patchCord16(mixer4, 0, reverb1, 1);
AudioConnection          patchCord17(mixer4, delay2);
AudioConnection          patchCord18(reverb1, 0, i2s1, 0);
AudioConnection          patchCord19(reverb1, 1, i2s1, 1);
#ifdef AUDIO_INTERFACE
AudioConnection          patchCord20(reverb1, 0, usb1, 0);
AudioConnection          patchCord21(reverb1, 1, usb1, 1);
#endif

AudioControlSGTL5000     sgtl5000_1;     //xy=706,445
// GUItool: end automatically generated code

float sgtl_volume;
size_t top_rotor, bottom_rotor;
AudioSynthMutableFMDrum *top_drums[] = { &top1, &top2, &top3, &top4 };
AudioSynthMutableFMDrum *bottom_drums[] = {
  &bottom1, &bottom2, &bottom3, &bottom4
};
size_t top_count = 4, bottom_count = 4;

static float scale_f2i(float f) {
  return max(0, min(65535, (int)(65535.0f * f)));
}

void AudioManager::trigger_launch(float frequency, float duration) {
  launch1.setFrequency(scale_f2i(frequency));
  launch1.setDecay(scale_f2i(duration));
  launch1.noteOn();
}

void
AudioManager::trigger_top_bounce(float volume,
                                 float frequency,
                                 float duration) {
  auto *drum = top_drums[top_rotor];
  mixer1.gain(top_rotor, volume);
  drum->setFrequency(scale_f2i(frequency));
  drum->setDecay(scale_f2i(duration));
  drum->noteOn();
  top_rotor = (top_rotor + 1) % top_count;
}

void
AudioManager::trigger_bottom_bounce(float volume,
                                    float frequency,
                                    float duration) {
  auto *drum = bottom_drums[bottom_rotor];
  mixer2.gain(bottom_rotor, volume);
  drum->setFrequency(scale_f2i(frequency));
  drum->setDecay(scale_f2i(duration));
  drum->noteOn();
  bottom_rotor = (bottom_rotor + 1) % bottom_count;
}

void AudioManager::setup() {
  AudioMemory(120);

  sgtl_volume = 0.0;
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.0);  // ramp up later.

  top1.setFrequency(32768);
  top1.setFMAmount(32768);
  top1.setDecay(32768);
  top1.setNoise(15000);

  top2.setFrequency(32768);
  top2.setFMAmount(32768);
  top2.setDecay(32768);
  top2.setNoise(15000);

  top3.setFrequency(32768);
  top3.setFMAmount(32768);
  top3.setDecay(32768);
  top3.setNoise(15000);

  top4.setFrequency(32768);
  top4.setFMAmount(32768);
  top4.setDecay(32768);
  top4.setNoise(15000);

  bottom1.setFrequency(32768);
  bottom1.setFMAmount(32768);
  bottom1.setDecay(32768);
  bottom1.setNoise(15000);

  bottom2.setFrequency(32768);
  bottom2.setFMAmount(32768);
  bottom2.setDecay(32768);
  bottom2.setNoise(15000);

  bottom3.setFrequency(32768);
  bottom3.setFMAmount(32768);
  bottom3.setDecay(32768);
  bottom3.setNoise(15000);

  bottom4.setFrequency(32768);
  bottom4.setFMAmount(32768);
  bottom4.setDecay(32768);
  bottom4.setNoise(15000);

  launch1.setFrequency(0);
  launch1.setPunch(4000);
  launch1.setTone(32768);
  launch1.setDecay(32768);

  mixer1.gain(0, 0.25);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.25);

  mixer2.gain(0, 0.25);
  mixer2.gain(1, 0.25);
  mixer2.gain(2, 0.25);
  mixer2.gain(3, 0.25);

  mixer3.gain(0, 0.4);
  mixer3.gain(1, 0.7);

  mixer4.gain(0, 0.4);
  mixer4.gain(1, 0.7);
  mixer4.gain(2, 1.0);

  delay1.delay(0, 80);
  delay2.delay(0, 100);

  reverb1.inputGain(0.5);
  reverb1.amount(0.15);
  reverb1.time(0.2);
}

void AudioManager::loop() {
  if (sgtl_volume < 0.5) {
    static uint32_t prev_adjust;
    uint32_t now = millis();
    if (prev_adjust < now) {
      prev_adjust = now;
      sgtl_volume += 0.001;
      if (sgtl_volume > 0.5)
        sgtl_volume = 0.5;
      sgtl5000_1.volume(sgtl_volume);
    }
  }

  static uint32_t print_time;
  uint32_t now = millis();
  if (now - print_time > 1000) {
    // Serial.printf("Audio Memory: %d, max %d\n",
    //               AudioMemoryUsage(), AudioMemoryUsageMax());
    print_time += 1000;
  }
}
