#include "AudioSynthMutableSnareDrum.h"

#include "snare_drum.h"

static Parameter snare_params[] = {
  // group.member         min     max  default
  { "snare.tone",           0,  65535,      0 },
  { "snare.snappy",         0,  28672,  28672 },
  { "snare.decay",          0,  65535,  32767 },
  { "snare.frequency", -32768, +32767,      0 },
};
static const size_t snare_param_count = (&snare_params)[1] - snare_params;
static Parameter::value_type snare_param_values[snare_param_count];

AudioSynthMutableSnareDrum::AudioSynthMutableSnareDrum()
: AudioStream(0, inputQueueArray),
  Parameterized(snare_params, snare_param_values, snare_param_count),
  cpu_usec(0),
  blocks_processed(0),
  note_on(false)
{
  snare.Init();
  Parameterized::init();
}

void AudioSynthMutableSnareDrum::update(void) {
  elapsedMicros elapsed;

  // Hack: [0..n) has a trigger; [1..n] has none.
  static const peaks::GateFlags gate_flags[AUDIO_BLOCK_SAMPLES + 1] = {
    peaks::GATE_FLAG_RISING,
  };

  audio_block_t *out = allocate();
  if (!out) {
    return;
  }
  snare.Process(gate_flags + !note_on, out->data, AUDIO_BLOCK_SAMPLES);
  note_on = false;
  transmit(out, 0);
  release(out);

  cpu_usec += elapsed;
  blocks_processed++;
}

void AudioSynthMutableSnareDrum::set(const char *group_member, Parameter::value_type new_value) {
  if (!Parameterized::save_value(group_member, new_value)) {
    return;
  }
  if (!strcmp(group_member, "snare.tone")) {
    setTone(new_value);
  }
  if (!strcmp(group_member, "snare.snappy")) {
    setSnappy(new_value);
  }
  if (!strcmp(group_member, "snare.decay")) {
    setDecay(new_value);
  }
  if (!strcmp(group_member, "snare.frequency")) {
    setFrequency(new_value);
  }
}

void AudioSynthMutableSnareDrum::noteOn() {
  __disable_irq();
  {
    note_on = true;
  }
  __enable_irq();
}

void AudioSynthMutableSnareDrum::setTone(int tone) {
  __disable_irq();
  {
    snare.set_tone(tone);
  }
  __enable_irq();
}

void AudioSynthMutableSnareDrum::setSnappy(int snappy) {
  __disable_irq();
  {
    snare.set_snappy(snappy);
  }
  __enable_irq();
}

void AudioSynthMutableSnareDrum::setDecay(int decay) {
  __disable_irq();
  {
    snare.set_decay(decay);
  }
  __enable_irq();
}

void AudioSynthMutableSnareDrum::setFrequency(int freq) {
  __disable_irq();
  {
    snare.set_frequency(freq);
  }
  __enable_irq();
}

float AudioSynthMutableSnareDrum::cpu() {
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
