#include <Arduino.h>
#include <AudioStream.h>

#include "snare_drum.h"

class AudioSynthMutableSnareDrum : public AudioStream
{
public:
  AudioSynthMutableSnareDrum() : AudioStream(0, inputQueueArray) {
    // any extra initialization
    snare.Init();
  }

  virtual void update(void);

  void noteOn(void);

  void setFrequency(float);
  void setDecay(float);
  void setTone(float);
  void setSnappy(float);

private:
  audio_block_t *inputQueueArray[0];
  peaks::SnareDrum snare;
};