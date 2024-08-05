#include "AudioSynthMutableSnareDrum.h"
#include "AudioEffectMutableReverb.h"

AudioSynthMutableSnareDrum snare;
AudioEffectMutableReverb reverb;

typedef struct float_param {
  const char *group_member;
  const float min;
  const float max;
  const float default_;
  const char *units;
  float value;
  bool changed;
} float_param;
float_param params[] = {
  { "snare.tone",        0.0,  1.0,  0.0 },
  { "snare.snappy",      0.0,  1.0,  0.5 },
  { "snare.decay",       0.0,  1.0,  0.5 },
  { "snare.frequency",   0.0,  1.0,  0.0 },

  { "reverb.input_gain", 0.0,  1.0,  0.2 },
  { "reverb.time",       0.35, 1.0,  0.0 },
  { "reverb.diffusion",  0.0,  1.0,  0.7 },
  { "reverb.low_pass",   0.6,  1.0,  0.7 },
  { "reverb.amount",     0.0,  0.54, 0.0 },
};
static const size_t param_count = (&params)[1] - params;

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:

}
