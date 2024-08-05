#include <Arduino.h>
#include <AudioStream.h>

#include "Parameterized.h"
#include "fm_drum.h"

#ifndef AUDIO_SYNTH_MUTABLE_FM_DRUM_included
#define AUDIO_SYNTH_MUTABLE_FM_DRUM_included

class AudioSynthMutableFMDrum: public AudioStream, public Parameterized
{
public:
  AudioSynthMutableFMDrum();

  virtual void update(void);

  virtual void set(const char *group_member, Parameter::value_type value);

  void noteOn(void);

  void setFrequency(int);
  void setFMAmount(int);
  void setDecay(int);
  void setNoise(int);

  float cpu();

private:
  audio_block_t *inputQueueArray[0];
  uint64_t cpu_usec;
  uint32_t blocks_processed;
  bool note_on;
  peaks::FmDrum fm_drum;
};

#endif /* !AUDIO_SYNTH_MUTABLE_FM_DRUM_included */
