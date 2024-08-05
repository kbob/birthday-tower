#include <Arduino.h>
#include <AudioStream.h>

#include "Parameterized.h"
#include "bass_drum.h"

class AudioSynthMutableBassDrum : public AudioStream, public Parameterized
{
public:
  AudioSynthMutableBassDrum();

  virtual void update(void);

  virtual void set(const char *group_member, Parameter::value_type value);

  void noteOn(void);

  void setFrequency(int);
  void setPunch(int);
  void setTone(int);
  void setDecay(int);

  float cpu();

private:
  audio_block_t *inputQueueArray[0];
  uint64_t cpu_usec;
  uint32_t blocks_processed;
  bool note_on;
  peaks::BassDrum bass;
};