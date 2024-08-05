#include "AudioEffectMutableReverb.h"

#define F2V(f) ((Parameter::value_type)((f) * REVERB_SCALE_RESOLUTION))
#define V2F(v) ((float)(v) / (float)REVERB_SCALE_RESOLUTION)

static const Parameter reverb_params[] = {
  { "reverb.input_gain", 0, REVERB_SCALE_RESOLUTION, F2V(0.25)  },
  { "reverb.time",       0, REVERB_SCALE_RESOLUTION, F2V(1.0)   },
  { "reverb.diffusion",  0, REVERB_SCALE_RESOLUTION, F2V(0.625) },
  { "reverb.low_pass",   0, REVERB_SCALE_RESOLUTION, F2V(0.7)   },
  { "reverb.amount",     0, REVERB_SCALE_RESOLUTION, F2V(0.0)   },
};
static const size_t reverb_param_count = (&reverb_params)[1] - reverb_params;
static Parameter::value_type reverb_param_values[reverb_param_count];

AudioEffectMutableReverb::AudioEffectMutableReverb()
: AudioStream(2, inputQueueArray),
  Parameterized(reverb_params, reverb_param_values, reverb_param_count),
  cpu_usec(0),
  blocks_processed(0)
{
  reverb.Init(buffer);
  Parameterized::init();
}

void AudioEffectMutableReverb::update(void) {
  elapsedMicros elapsed;
  float left[AUDIO_BLOCK_SAMPLES], right[AUDIO_BLOCK_SAMPLES];
  audio_block_t *left_in = receiveReadOnly(0);
  if (left_in) {
    for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      left[i] = left_in->data[i] / 32768.0f;
    }
    release(left_in);
  } else {
    bzero(left, sizeof left);
  }
  audio_block_t *right_in = receiveReadOnly(1);
  if (right_in) {
    for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      right[i] = right_in->data[i] / 32768.0f;
    }
    release(right_in);
  } else {
    bzero(right, sizeof right);
  }

  reverb.Process(left, right, AUDIO_BLOCK_SAMPLES);

  audio_block_t *left_out = allocate();
  if (left_out) {
    for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      left_out->data[i] = stmlib::Clip16(left[i] * 32768.0f);
    }
    transmit(left_out, 0);
    release(left_out);
  }
  audio_block_t *right_out = allocate();
  if (right_out) {
    for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      right_out->data[i] = stmlib::Clip16(right[i] * 32768.0f);
    }
    transmit(right_out, 1);
    release(right_out);
  }
  cpu_usec += elapsed;
  blocks_processed++;
}

void
AudioEffectMutableReverb::set(const char *group_member, Parameter::value_type new_value) {
  if (!Parameterized::save_value(group_member, new_value)) {
    return;
  }
  if (!strcmp(group_member, "reverb.input_gain")) {
    inputGain(V2F(new_value));
  }
  if (!strcmp(group_member, "reverb.time")) {
    time(V2F(new_value));
  }
  if (!strcmp(group_member, "reverb.diffusion")) {
    diffusion(V2F(new_value));
  }
  if (!strcmp(group_member, "reverb.low_pass")) {
    lowPass(V2F(new_value));
  }
  if (!strcmp(group_member, "reverb.amount")) {
    amount(V2F(new_value));
  }
}

void AudioEffectMutableReverb::amount(float amount) {
  __disable_irq();
  Serial.printf("amount = %g\n", amount);
  reverb.set_amount(amount);
  __enable_irq();
}

void AudioEffectMutableReverb::inputGain(float input_gain) {
  __disable_irq();
  Serial.printf("input_gain = %g\n", input_gain);
  reverb.set_input_gain(input_gain);
  __enable_irq();
}

void AudioEffectMutableReverb::time(float time) {
  __disable_irq();
  reverb.set_time(time);
  __enable_irq();
}

void AudioEffectMutableReverb::diffusion(float diffusion) {
  __disable_irq();
  reverb.set_diffusion(diffusion);
  __enable_irq();
}

void AudioEffectMutableReverb::lowPass(float freq) {
  __disable_irq();
  reverb.set_lp(freq);
  __enable_irq();
}

float AudioEffectMutableReverb::cpu() {
  uint64_t cpu;
  uint32_t blks;
  __disable_irq();
  {
    cpu = cpu_usec;
    blks = blocks_processed;
    cpu_usec = 0;
    blocks_processed = 0;
  }
  __enable_irq();
  const float usec_per_block = 1e6 * AUDIO_BLOCK_SAMPLES / AUDIO_SAMPLE_RATE_EXACT;
  return cpu / (blks * usec_per_block);
}
