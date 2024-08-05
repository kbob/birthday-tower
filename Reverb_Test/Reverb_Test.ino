#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

#include "AudioEffectMutableReverb.h"
#include "AudioSynthMutableBassDrum.h"
#include "AudioSynthMutableFMDrum.h"
#include "AudioSynthMutableSnareDrum.h"
#include "AudioSampleLeft.h"
#include "AudioSampleRight.h"
#include "StreamReader.h"

#define PLOTTER_STREAM Serial
#define COMMAND_STREAM SerialUSB1

// GUItool: begin automatically generated code
AudioPlayMemory          playMem1;       //xy=65,396
AudioSynthMutableBassDrum drum2;         //xy=67,597
AudioPlayMemory          playMem2;       //xy=79,542
AudioSynthMutableSnareDrum drum1;        //xy=80,435
AudioSynthMutableFMDrum  drum3;          //xy=80,475
AudioMixer4              mixer1;         //xy=250,375
AudioMixer4              mixer2;         //xy=250,605
AudioEffectMutableReverb reverb1;        //xy=323,476
AudioOutputI2S           i2s1;           //xy=490,481
AudioConnection          patchCord1(playMem1, 0, mixer1, 0);
AudioConnection          patchCord2(drum2, 0, mixer2, 1);
AudioConnection          patchCord3(playMem2, 0, mixer2, 0);
AudioConnection          patchCord4(drum1, 0, mixer1, 1);
AudioConnection          patchCord5(drum3, 0, mixer1, 2);
AudioConnection          patchCord6(mixer1, 0, reverb1, 0);
AudioConnection          patchCord7(mixer2, 0, reverb1, 1);
AudioConnection          patchCord8(reverb1, 0, i2s1, 0);
AudioConnection          patchCord9(reverb1, 1, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=481,752
// GUItool: end automatically generated code

Parameterized *parameterized_things[] = {
  // &drum1,
  &drum2,
  // &drum3,
  &reverb1,
};
const size_t pthing_count = (&parameterized_things)[1] - parameterized_things;


StreamReader command_reader(COMMAND_STREAM);
uint32_t next_time;

void exec_list_command(const char *) {
  for (size_t i = 0; i < pthing_count; i++) {
    const Parameterized *ptp = parameterized_things[i];
    for (size_t j = 0; j < ptp->parameter_count(); j++) {
      const Parameter *p = ptp->describe(j);
      COMMAND_STREAM.printf("%s %d %d %d %s\n",
        p->group_member, p->min, p->max, p->default_, p->units);
    }
  }
  COMMAND_STREAM.printf("-\n");
}

void exec_get_command(const char *cmd) {
  char group_member[40];
  int n = sscanf(cmd, "get %39s\n", group_member);
  if (n != 1) {
    COMMAND_STREAM.printf("get: missing parameter name\n");
    return;
  }
  for (size_t i = 0; i < pthing_count; i++) {
    Parameterized *ptp = parameterized_things[i];
    for (size_t j = 0; j < ptp->parameter_count(); j++) {
      const Parameter *p = ptp->describe(j);
      if (!strcmp(p->group_member, group_member)) {
        COMMAND_STREAM.printf("%s=%d\n", group_member, ptp->get(group_member));
        return;
      }
    }
  }
  COMMAND_STREAM.printf("Parameter %s not found.\n", group_member);
}

void exec_set_command(const char *cmd) {
  char group_member[40];
  int new_value;
  int n = sscanf(cmd, "set %39s %d\n", group_member, &new_value);
  if (n != 2) {
    COMMAND_STREAM.printf("get: missing parameter name\n");
    return;
  }
  for (size_t i = 0; i < pthing_count; i++) {
    Parameterized *ptp = parameterized_things[i];
    for (size_t j = 0; j < ptp->parameter_count(); j++) {
      const Parameter *p = ptp->describe(j);
      if (!strcmp(p->group_member, group_member)) {
        if (new_value < p->min || new_value > p->max) {
          COMMAND_STREAM.printf("%s: value %g is out of range.",
                                group_member, new_value);
        } else {
          ptp->set(group_member, new_value);
          COMMAND_STREAM.printf("OK\n");
        }
        return;
      }
    }
  }
  COMMAND_STREAM.printf("Parameter %s not found.\n", group_member);
}

void exec_command(const char *cmd) {
  size_t len = strlen(cmd);
  while (len > 0 && (cmd[len - 1] == '\r' || cmd[len - 1] == '\n')) {
    len--;
  }
  if (len == 0)
    return;
  if (!strncmp("list", cmd, 4)) {
    exec_list_command(cmd);
  } else if (!strncmp("get ", cmd, 4)) {
    exec_get_command(cmd);
  } else if (!strncmp("set ", cmd, 4)) {
    exec_set_command(cmd);
  } else
  COMMAND_STREAM.printf("Unknown command: \"%.*s\"\n", len, cmd);
}

void setup() {
  COMMAND_STREAM.begin(9600);
  // PLOTTER_STREAM.begin(9600);

  pinMode(0, INPUT_PULLUP);

  AudioMemory(200);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  
  mixer1.gain(0, 1.0);
  mixer1.gain(1, 1.0);
  mixer1.gain(3, 0.8);
  mixer2.gain(0, 1.0);
  mixer2.gain(3, 0.8);

  next_time = (millis() + 499) / 500 * 500;
}

void loop() {

  if (command_reader.collect_input()) {
    const char *cmd = command_reader.current_line();
    exec_command(cmd);
    command_reader.clear_line();
  }

  uint32_t now = millis();
  if (now >= next_time) {
    int beat = next_time % 2000 / 500;
    next_time += 500;
    switch(beat) {

    case 0:
      Serial.printf("CPU utilization: reverb %3.2g%%, bass %3.2g%%, "
                    "snare %3.2g%%, FM drum %3.2g%%\n",
        100 * reverb1.cpu(),
        100 * drum2.cpu(),
        100 * drum1.cpu(),
        100 * drum3.cpu());
      drum2.noteOn();
      break;

    case 1:
      // playMem2.play(AudioSampleRight);
      drum2.noteOn();
      break;

    case 2:
      drum2.noteOn();
      break;

    case 3:
      // drum2.length(200);
      // drum2.noteOn();
      drum2.noteOn();
      break;
    }
  }
}
