#include "AudioSynthMutableBassDrum.h"

#include "bass_drum.h"

static Parameter bass_params[] = {
  { "bass.frequency", -32768, +32767,      0 },
  { "bass.punch",          0,  65535,  65535 },
  { "bass.tone",           0,  65535,  32768 },
  { "bass.decay",          0,  65535,  32768 },
};
static const size_t bass_param_count = (&bass_params)[1] - bass_params;
static Parameter::value_type bass_param_values[bass_param_count];

AudioSynthMutableBassDrum::AudioSynthMutableBassDrum()
: AudioStream(0, inputQueueArray),
  Parameterized(bass_params, bass_param_values, bass_param_count),
  cpu_usec(0),
  blocks_processed(0),
  note_on(false)
{
  bass.Init();
  Parameterized::init();
}

void AudioSynthMutableBassDrum::update(void) {
  elapsedMicros elapsed;

  // Hack: [0..n) has a trigger; [1..n] has none.
  static const peaks::GateFlags gate_flags[AUDIO_BLOCK_SAMPLES + 1] = {
    peaks::GATE_FLAG_RISING,
  };

  audio_block_t *out = allocate();
  if (!out) {
    return;
  }
  bass.Process(gate_flags + !note_on, out->data, AUDIO_BLOCK_SAMPLES);
  note_on = false;
  transmit(out, 0);
  release(out);

  cpu_usec += elapsed;
  blocks_processed++;
}

void AudioSynthMutableBassDrum::set(const char *group_member, Parameter::value_type new_value) {
  if (!Parameterized::save_value(group_member, new_value)) {
    return;
  }
  if (!strcmp(group_member, "bass.frequency")) {
    setFrequency(new_value);
  }
  if (!strcmp(group_member, "bass.punch")) {
    setPunch(new_value);
  }
  if (!strcmp(group_member, "bass.tone")) {
    setTone(new_value);
  }
  if (!strcmp(group_member, "bass.decay")) {
    setDecay(new_value);
  }
}

void AudioSynthMutableBassDrum::noteOn() {
  __disable_irq();
  {
    note_on = true;
  }
  __enable_irq();
}

void AudioSynthMutableBassDrum::setFrequency(int freq) {
  __disable_irq();
  {
    bass.set_frequency(freq);
  }
  __enable_irq();
}

void AudioSynthMutableBassDrum::setPunch(int punch) {
  __disable_irq();
  {
    bass.set_punch(punch);
  }
  __enable_irq();
}

void AudioSynthMutableBassDrum::setTone(int tone) {
  __disable_irq();
  {
    bass.set_tone(tone);
  }
  __enable_irq();
}

void AudioSynthMutableBassDrum::setDecay(int decay) {
  __disable_irq();
  {
    bass.set_decay(decay);
  }
  __enable_irq();
}

float AudioSynthMutableBassDrum::cpu() {
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
