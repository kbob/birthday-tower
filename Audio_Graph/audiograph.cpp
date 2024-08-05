#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthSimpleDrum     drum1;          //xy=75,40
AudioSynthSimpleDrum     drum2;          //xy=75,80
AudioSynthSimpleDrum     drum3;          //xy=75,120
AudioSynthSimpleDrum     drum4;          //xy=75,160

AudioSynthSimpleDrum     drum5;          //xy=75,370
AudioSynthSimpleDrum     drum6;          //xy=75,410
AudioSynthSimpleDrum     drum7;          //xy=75,450
AudioSynthSimpleDrum     drum8;          //xy=75,490

AudioSynthKarplusStrong  string1;        //xy=75,570

AudioMixer4              mixer1;         //xy=235,100
AudioMixer4              mixer2;         //xy=235,430

AudioMixer4              mixer3;         //xy=395,100
AudioEffectDelay         delay1;         //xy=395,200
AudioEffectDelay         delay2;         //xy=395,330
AudioMixer4              mixer4;         //xy=395,430

AudioEffectMidSide       midside1;       //xy=555,265

AudioOutputI2S           i2s1;           //xy=715,245
AudioOutputUSB           usb1;           //xy=715,285

AudioConnection          patchCord1(drum1, 0, mixer1, 0);
AudioConnection          patchCord2(drum2, 0, mixer1, 1);
AudioConnection          patchCord3(drum3, 0, mixer1, 2);
AudioConnection          patchCord4(drum4, 0, mixer1, 3);
AudioConnection          patchCord5(drum5, 0, mixer2, 0);
AudioConnection          patchCord6(drum6, 0, mixer2, 1);
AudioConnection          patchCord7(drum7, 0, mixer2, 2);
AudioConnection          patchCord8(drum8, 0, mixer2, 3);
AudioConnection          patchCord9(string1, 0, mixer4, 2);
AudioConnection          patchCord10(mixer1, 0, mixer3, 0);
AudioConnection          patchCord11(mixer2, 0, mixer4, 0);
AudioConnection          patchCord12(mixer3, 0, midside1, 0);
AudioConnection          patchCord13(mixer3, delay1);
AudioConnection          patchCord14(delay1, 0, mixer4, 1);
AudioConnection          patchCord15(delay2, 0, mixer3, 1);
AudioConnection          patchCord16(mixer4, 0, midside1, 1);
AudioConnection          patchCord17(mixer4, delay2);
AudioConnection          patchCord18(midside1, 0, i2s1, 0);
AudioConnection          patchCord19(midside1, 0, usb1, 0);
AudioConnection          patchCord20(midside1, 1, i2s1, 1);
AudioConnection          patchCord21(midside1, 1, usb1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=706,445
// GUItool: end automatically generated code
