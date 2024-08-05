#include "AudioSampleLeft.h"
#include "AudioSampleRight.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlayMemory          playMem1;       //xy=65,396
AudioPlayMemory          playMem2;       //xy=79,542
AudioSynthSimpleDrum     drum1;          //xy=80,435
AudioSynthSimpleDrum     drum2;          //xy=101,595
AudioMixer4              mixer1;         //xy=250,375
AudioMixer4              mixer2;         //xy=250,605
AudioEffectDelay         delay1;         //xy=430,345
AudioEffectDelay         delay2;         //xy=430,635
AudioOutputI2S           i2s1;           //xy=490,481
AudioConnection          patchCord1(playMem1, 0, mixer1, 0);
AudioConnection          patchCord2(playMem2, 0, mixer2, 0);
AudioConnection          patchCord3(drum1, 0, mixer1, 1);
AudioConnection          patchCord4(drum2, 0, mixer2, 1);
AudioConnection          patchCord5(mixer1, delay1);
AudioConnection          patchCord6(mixer1, 0, i2s1, 0);
AudioConnection          patchCord7(mixer2, delay2);
AudioConnection          patchCord8(mixer2, 0, i2s1, 1);
AudioConnection          patchCord9(delay1, 0, mixer2, 3);
AudioConnection          patchCord10(delay2, 0, mixer1, 3);
AudioControlSGTL5000     sgtl5000_1;     //xy=481,752
// GUItool: end automatically generated code

uint32_t next_time;

void setup()
{
  pinMode(0, INPUT_PULLUP);

  AudioMemory(200);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  sgtl5000_1.unmuteLineout();
  sgtl5000_1.lineOutLevel(29);
  mixer1.gain(0, 1.0);
  mixer1.gain(3, 0.8);
  mixer2.gain(0, 1.0);
  mixer2.gain(3, 0.8);
  delay1.delay(0, 75);
  delay2.delay(0, 75);

  next_time = (millis() + 499) / 500 * 500;
}

void loop()
{
  uint32_t now = millis();
  if (now >= next_time) {
    int beat = next_time % 2000 / 500;
    next_time += 500;
    switch(beat) {
      case 0:
        if (digitalRead(0)) {
          mixer1.gain(3, 0.8);
          mixer2.gain(3, 0.8);
        } else {
          mixer1.gain(3, 0.0);
          mixer2.gain(3, 0.0);
        }
        playMem1.play(AudioSampleLeft);
        break;
      case 1:
        playMem2.play(AudioSampleRight);
        break;
      case 2:
        // drum1.frequency(random(40, 400));
        drum1.frequency(80);
        // drum1.pitchMod(0.5);
        drum1.length(200);
        drum1.noteOn();
        break;
      case 3:
        drum2.length(200);
        drum2.noteOn();
        break;
    }
  }
}