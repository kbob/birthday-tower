#include <Arduino.h>
#include <AudioStream.h>

#include "reverb.h"

class AudioEffectMutableReverb : public AudioStream {
public:
  AudioEffectMutableReverb()
    : AudioStream(2, inputQueueArray) {
    // any extra initialization
    reverb.Init(buffer);
  }

  virtual void update(void);

  void amount(float);
  void inputGain(float);
  void time(float);
  void diffusion(float);
  void lowPass(float);

private:
  typedef uint16_t STORAGE_TYPE;

  audio_block_t *inputQueueArray[2];
  elements::Reverb reverb;
  STORAGE_TYPE buffer[elements::Reverb::SIZE];
};