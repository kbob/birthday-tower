#include "AudioSynthMutableFMDrum.h"

#include "fm_drum.h"

static Parameter fm_drum_params[] = {
  { "fm_drum.frequency",      0,  65535,  32768 },
  { "fm_drum.fm_amount",      0,  65535,  32768 },
  { "fm_drum.decay",          0,  65535,  32768 },
  { "fm_drum.noise",          0,  65535,  32768 },
};
static const size_t fm_drum_param_count = (&fm_drum_params)[1] - fm_drum_params;
static Parameter::value_type fm_drum_param_values[fm_drum_param_count];

AudioSynthMutableFMDrum::AudioSynthMutableFMDrum()
: AudioStream(0, inputQueueArray),
  Parameterized(fm_drum_params, fm_drum_param_values, fm_drum_param_count),
  cpu_usec(0),
  blocks_processed(0),
  note_on(false)
{
  fm_drum.Init();
  Parameterized::init();
}

void AudioSynthMutableFMDrum::update(void) {
  elapsedMicros elapsed;

  // Hack: [0..n) has a trigger; [1..n] has none.
  static const peaks::GateFlags gate_flags[AUDIO_BLOCK_SAMPLES + 1] = {
    peaks::GATE_FLAG_RISING,
  };

  audio_block_t *out = allocate();
  if (!out) {
    return;
  }
  fm_drum.Process(gate_flags + !note_on, out->data, AUDIO_BLOCK_SAMPLES);
  note_on = false;
  transmit(out, 0);
  release(out);

  cpu_usec += elapsed;
  blocks_processed++;
}

void AudioSynthMutableFMDrum::set(const char *group_member, Parameter::value_type new_value) {
  if (!Parameterized::save_value(group_member, new_value)) {
    return;
  }
  if (!strcmp(group_member, "fm_drum.frequency")) {
    setFrequency(new_value);
  }
  if (!strcmp(group_member, "fm_drum.fm_amount")) {
    setFMAmount(new_value);
  }
  if (!strcmp(group_member, "fm_drum.decay")) {
    setDecay(new_value);
  }
  if (!strcmp(group_member, "fm_drum.noise")) {
    setNoise(new_value);
  }
}

void AudioSynthMutableFMDrum::noteOn() {
  __disable_irq();
  {
    note_on = true;
  }
  __enable_irq();
}

void AudioSynthMutableFMDrum::setFrequency(int freq) {
  __disable_irq();
  {
    fm_drum.set_frequency(freq);
  }
  __enable_irq();
}


void AudioSynthMutableFMDrum::setFMAmount(int amount) {
  __disable_irq();
  {
    fm_drum.set_fm_amount(amount);
  }
  __enable_irq();
}

void AudioSynthMutableFMDrum::setDecay(int decay) {
  __disable_irq();
  {
    fm_drum.set_decay(decay);
  }
  __enable_irq();
}

void AudioSynthMutableFMDrum::setNoise(int noise) {
  __disable_irq();
  {
    fm_drum.set_noise(noise);
  }
  __enable_irq();
}

float AudioSynthMutableFMDrum::cpu() {
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
