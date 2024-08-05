#include "AudioEffectMutableReverb.h"

void AudioEffectMutableReverb::update(void) {
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
}

void AudioEffectMutableReverb::amount(float amount) {
  __disable_irq();
  reverb.set_amount(amount);
  __enable_irq();
}

void AudioEffectMutableReverb::inputGain(float input_gain) {
  __disable_irq();
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
