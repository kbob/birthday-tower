#include <Arduino.h>
#include <AudioStream.h>

#include "reverb.h"

#include "Parameterized.h"

#define REVERB_SCALE_RESOLUTION 1000

class AudioEffectMutableReverb : public AudioStream, public Parameterized {

public:
  AudioEffectMutableReverb();

  // override AudioStream
  virtual void update(void);

  // override Parameterized
  virtual void set(const char *group_member, Parameter::value_type value);

  void amount(float);
  void inputGain(float);
  void time(float);
  void diffusion(float);
  void lowPass(float);

  float cpu();

private:
  typedef uint16_t STORAGE_TYPE;

  audio_block_t *inputQueueArray[2];
  uint64_t cpu_usec;
  uint32_t blocks_processed;
  elements::Reverb reverb;
  STORAGE_TYPE buffer[elements::Reverb::SIZE];
};