#include <Arduino.h>
#include <AudioStream.h>

#include "Parameterized.h"
#include "snare_drum.h"

class AudioSynthMutableSnareDrum : public AudioStream, public Parameterized
{
public:
  AudioSynthMutableSnareDrum();

  virtual void update(void);

  virtual void set(const char *group_member, Parameter::value_type value);

  void noteOn(void);

  void setTone(int);
  void setSnappy(int);
  void setDecay(int);
  void setFrequency(int);

  float cpu();

private:
  audio_block_t *inputQueueArray[0];
  uint64_t cpu_usec;
  uint32_t blocks_processed;
  bool note_on;
  peaks::SnareDrum snare;
};