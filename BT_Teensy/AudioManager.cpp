#include "AudioManager.h"

#include <Audio.h>

#include "AudioSynthMutableBassDrum.h"
#include "AudioSynthMutableFMDrum.h"
#include "AudioEffectMutableReverb.h"
#include "CommandProcessor.h"
#include "Sigmap.h"
#include "StreamReader.h"

// GUItool: begin automatically generated code
AudioSynthMutableFMDrum  top1;           //xy=75,40
AudioSynthMutableFMDrum  top2;           //xy=75,80
AudioSynthMutableFMDrum  top3;           //xy=75,120
AudioSynthMutableFMDrum  top4;           //xy=75,160

AudioSynthMutableFMDrum  bottom1;        //xy=75,370
AudioSynthMutableFMDrum  bottom2;        //xy=75,410
AudioSynthMutableFMDrum  bottom3;        //xy=75,450
AudioSynthMutableFMDrum  bottom4;        //xy=75,490

AudioSynthMutableBassDrum launch1;       //xy=75,570

AudioMixer4              mixer1;         //xy=235,100
AudioMixer4              mixer2;         //xy=235,430

AudioMixer4              mixer3;         //xy=395,100
AudioEffectDelay         delay1;         //xy=395,200
AudioEffectDelay         delay2;         //xy=395,330
AudioMixer4              mixer4;         //xy=395,430

AudioEffectMutableReverb reverb1;        //xy=555,265

AudioOutputI2S           i2s1;           //xy=715,245
#ifdef AUDIO_INTERFACE
AudioOutputUSB           usb1;           //xy=715,285
#endif

AudioConnection          patchCord1(top1, 0, mixer1, 0);
AudioConnection          patchCord2(top2, 0, mixer1, 1);
AudioConnection          patchCord3(top3, 0, mixer1, 2);
AudioConnection          patchCord4(top4, 0, mixer1, 3);
AudioConnection          patchCord5(bottom1, 0, mixer2, 0);
AudioConnection          patchCord6(bottom2, 0, mixer2, 1);
AudioConnection          patchCord7(bottom3, 0, mixer2, 2);
AudioConnection          patchCord8(bottom4, 0, mixer2, 3);
AudioConnection          patchCord9(launch1, 0, mixer4, 2);
AudioConnection          patchCord10(mixer1, 0, mixer3, 0);
AudioConnection          patchCord11(mixer2, 0, mixer4, 0);
AudioConnection          patchCord12(mixer3, 0, reverb1, 0);
AudioConnection          patchCord13(mixer3, delay1);
AudioConnection          patchCord14(delay1, 0, mixer4, 1);
AudioConnection          patchCord15(delay2, 0, mixer3, 1);
AudioConnection          patchCord16(mixer4, 0, reverb1, 1);
AudioConnection          patchCord17(mixer4, delay2);
AudioConnection          patchCord18(reverb1, 0, i2s1, 0);
AudioConnection          patchCord19(reverb1, 1, i2s1, 1);
#ifdef AUDIO_INTERFACE
AudioConnection          patchCord20(reverb1, 0, usb1, 0);
AudioConnection          patchCord21(reverb1, 1, usb1, 1);
#endif

AudioControlSGTL5000     sgtl5000_1;     //xy=706,445
// GUItool: end automatically generated code

static const Parameter manager_params[] = {
  // N.B., launch frequency is (signed) int16_t, but
  // bounce frequency is unsigned uint16_t.
  // Also, both launch and bounce duration min > max by default.
  // Faster particles trigger longer sounds.  I dunno why.
  { "launch.center_velocity", 40000, 120000,  40000 },
  { "launch.slope",               0,    100,     50, "percent" },
  { "launch.min_freq",       -32768, +32767, -32768 },
  { "launch.max_freq",       -32767, +99999, +49152 },
  { "launch.min_duration",        0,  65535,  65535 },
  { "launch.max_duration",        0,  65535,  16384 },

  { "bounce.center_velocity", 40000, 120000,  65000 },
  { "bounce.slope",               0,    100,     65, "percent" },
  { "bounce.min_freq",       -32768, +32767,      0 },
  { "bounce.max_freq",       -32768, +99999, +80000 },
  { "bounce.min_duration",        0,  65535,    255 },
  { "bounce.max_duration",        0,  65535,      0 },

  { "bass.punch",                 0,  65535,   4000 },
  { "bass.tone",                  0,  65535,  32767 },

  { "FM_drums.noise",             0,  65535,  15000 },
  { "FM_drums.FM_amount",         0,  65535,  32767 },

  { "mix.launch",                 0,    100,     30, "percent" },
  { "mix.bounce",                 0,    100,     50, "percent" },
  { "mix.volume",                 0,    100,     50, "percent" },

  { "delay.time",                 1,    250,     90, "msec"    },
  { "delay.cadence",             10,     90,     40, "percent" },
  { "delay.echo",                 0,    100,     77, "percent" },

  { "reverb.time",                0,    100,     20, "percent" },
  { "reverb.amount",              0,    100,     15, "percent" },
};
static const size_t manager_param_count =
  (&manager_params)[1] - manager_params;
static Parameter::value_type param_values[manager_param_count];

// These Sigmaps define the mapping from velocity (see comment
// in AudioManager.h) to drum frequency and duration (decay).
// The mapping is a sigmoid function; it asymptotically approaches
// its min and max values.  The values are also hard clipped
// to [0..65535] in the AudioManger::trigger_* methods.

// SLOPE_MAGIC gives a reasonable range of slopes for the sigmoid
// functions.  It was chosen through trial and error.
// Higher slope makes frequency/duration more sensitive to button duration.

const float SLOPE_MAGIC = 3.5e-7;
Sigmap launch_freq_map;
Sigmap launch_duration_map;
Sigmap bounce_freq_map;
Sigmap bounce_duration_map;

class AudioManParameters : public Parameterized {

public:
  AudioManParameters()
  : Parameterized(manager_params, param_values, manager_param_count),
    enabled(false),
    delay_time_index(parameter_index("delay.time")),
    delay_cadence_index(parameter_index("delay.cadence"))
  {

  }

  void enable(bool on_off) {
    enabled = on_off;
  }

  void init() {
    Parameterized::init();
  }

  virtual void
  set(const char *group_member, Parameter::value_type new_value) {
    if (!Parameterized::save_value(group_member, new_value)) {
      return;
    }
    if (!enabled) {
      return;
    }    
    if (!strcmp(group_member, "launch.center_velocity")) {
      launch_freq_map.center_x = new_value;
      launch_duration_map.center_x = new_value;
    } else if (!strcmp(group_member, "launch.slope")) {
      float slope = new_value * SLOPE_MAGIC;
      launch_freq_map.slope_x = slope;
      launch_duration_map.slope_x = slope;
    } else if (!strcmp(group_member, "launch.min_freq")) {
      launch_freq_map.min_y = new_value;
    } else if (!strcmp(group_member, "launch.max_freq")) {
      launch_freq_map.max_y = new_value;
    } else if (!strcmp(group_member, "launch.min_duration")) {
      launch_duration_map.min_y = new_value;
    } else if (!strcmp(group_member, "launch.max_duration")) {
      launch_duration_map.max_y = new_value;
    } else if (!strcmp(group_member, "bounce.center_velocity")) {
      bounce_freq_map.center_x = new_value;
      bounce_duration_map.center_x = new_value;
    } else if (!strcmp(group_member, "bounce.slope")) {
      float slope = new_value * SLOPE_MAGIC;
      bounce_freq_map.slope_x = slope;
      bounce_duration_map.slope_x = slope;
    } else if (!strcmp(group_member, "bounce.min_freq")) {
      bounce_freq_map.min_y = new_value;
    } else if (!strcmp(group_member, "bounce.max_freq")) {
      bounce_freq_map.max_y = new_value;
    } else if (!strcmp(group_member, "bounce.min_duration")) {
      bounce_duration_map.min_y = new_value;
    } else if (!strcmp(group_member, "bounce.max_duration")) {
      bounce_duration_map.max_y = new_value;
    } else if (!strcmp(group_member, "mix.launch")) {
      mixer4.gain(2, new_value / 100.0f);
    } else if (!strcmp(group_member, "mix.bounce")) {
      mixer3.gain(0, new_value / 100.0f);
      mixer4.gain(0, new_value / 100.0f);
    } else if (!strcmp(group_member, "mix.volume")) {

      // SGTL5000 volume is weird.  Headphone volume is a nice linear
      // thing from 0 to 1.  But the line out volume goes from 13 = 3.12v
      // to 31 = 1.16v, with 29 = 1.29v as the default.
      //
      // We'll make the best of the situation by mapping volumes 0..0.4375
      // to 31 = 1.16v, and linearly mapping [0.4375..1.0] to [31..13].
      // That maps 0.5 (the default) to 16 = 1.29v (the default).

      int line_level;
      if (new_value <= 43) {
        line_level = 31;
      } else {
        line_level = 31 + map(new_value * 100, 4375, 10000, 31, 13);
      }
      sgtl5000_1.volume(new_value / 100.0f);
      sgtl5000_1.lineOutLevel(line_level);
    } else if (!strcmp(group_member, "delay.time")) {
      update_delay();
    } else if (!strcmp(group_member, "delay.cadence")) {
      update_delay();
    } else if (!strcmp(group_member, "delay.echo")) {
      float echo = new_value / 100.0f;
      mixer3.gain(1, echo);
      mixer4.gain(1, echo);
    } else if (!strcmp(group_member, "reverb.time")) {
      reverb1.time(new_value / 100.0f);
    } else if (!strcmp(group_member, "reverb.amount")) {
      reverb1.amount(new_value / 100.0f);
    } else if (!strcmp(group_member, "bass.punch")) {
      launch1.setPunch(new_value);
    } else if (!strcmp(group_member, "bass.tone")) {
      launch1.setTone(new_value);
    } else if (!strcmp(group_member, "FM_drums.noise")) {
      top1.setNoise(new_value);
      top2.setNoise(new_value);
      top3.setNoise(new_value);
      top4.setNoise(new_value);
      bottom1.setNoise(new_value);
      bottom2.setNoise(new_value);
      bottom3.setNoise(new_value);
      bottom4.setNoise(new_value);
    } else if (!strcmp(group_member, "FM_drums.FM_amount")) {
      top1.setFMAmount(new_value);
      top2.setFMAmount(new_value);
      top3.setFMAmount(new_value);
      top4.setFMAmount(new_value);
      bottom1.setFMAmount(new_value);
      bottom2.setFMAmount(new_value);
      bottom3.setFMAmount(new_value);
      bottom4.setFMAmount(new_value);
    }
  }

private:
  bool enabled;
  size_t delay_time_index, delay_cadence_index;

  void update_delay() {
    // Cadence adds "swing" to the delay.  50% is regular ping pong.
    // 0% means all the delay is from bottom to top; 100% is the opposite.
    //    top delay = 2 * time * cadence;
    //    bottom delay = 2 * time * (1 - cadence);
    // Will it sound interesting?  I don't know.

    float time_msec = get(delay_time_index);
    float cadence = get(delay_cadence_index) / 100.0f;
    float top_delay = 2.0f * time_msec * cadence;
    float bottom_delay = 2.0f * time_msec * (1.0f - cadence);
    delay1.delay(0, top_delay);
    delay2.delay(0, bottom_delay);
  }
};
static AudioManParameters audio_params;

static size_t top_rotor, bottom_rotor;
static AudioSynthMutableFMDrum *top_drums[] = {
  &top1, &top2, &top3, &top4
};
static AudioSynthMutableFMDrum *bottom_drums[] = {
  &bottom1, &bottom2, &bottom3, &bottom4
};
static size_t top_count = 4, bottom_count = 4;

#if defined(USB_DUAL_SERIAL) || defined(USB_TRIPLE_SERIAL)

  #define PROCESS_COMMANDS true
  #define COMMAND_STREAM SerialUSB1
  static StreamReader command_reader(COMMAND_STREAM);
  static CommandProcessor command_processor(COMMAND_STREAM);

#else

  #if defined(PROCESS_COMMANDS)
     #error "somebody already has this variable"
  #endif

#endif

void AudioManager::trigger_launch(int velocity) {
  int freq = launch_freq_map.map(velocity);
  // N.B., bass drum freq is signed.
  freq = max(-32768, min(+32767, freq));
  int dur = launch_duration_map.map(velocity);
  dur = max(0, min(65535, dur));
  launch1.setFrequency(freq);
  launch1.setDecay(dur);
  launch1.noteOn();
}

void AudioManager::trigger_top_bounce(float volume, int velocity) {
  int freq = bounce_freq_map.map(velocity);
  freq = max(0, min(65535, freq));
  int dur = bounce_duration_map.map(velocity);
  dur = max(0, min(65535, dur));
  auto *drum = top_drums[top_rotor];
  mixer1.gain(top_rotor, volume);
  drum->setFrequency(freq);
  drum->setDecay(dur);
  drum->noteOn();
  top_rotor = (top_rotor + 1) % top_count;
}

void AudioManager::trigger_bottom_bounce(float volume, int velocity) {
  int freq = bounce_freq_map.map(velocity);
  freq = max(0, min(65535, freq));
  int dur = bounce_duration_map.map(velocity);
  dur = max(0, min(65535, dur));
  auto *drum = bottom_drums[bottom_rotor];
  mixer1.gain(bottom_rotor, volume);
  drum->setFrequency(freq);
  drum->setDecay(dur);
  drum->noteOn();
  bottom_rotor = (bottom_rotor + 1) % bottom_count;
}

void AudioManager::setup() {
  AudioMemory(250);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  mixer1.gain(0, 0.25);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.25);

  mixer2.gain(0, 0.25);
  mixer2.gain(1, 0.25);
  mixer2.gain(2, 0.25);
  mixer2.gain(3, 0.25);

  mixer3.gain(0, 0.4);    // top bounce gain
  mixer3.gain(1, 0.7);    // delay gain

  mixer4.gain(0, 0.4);    // top bounce gain
  mixer4.gain(1, 0.7);    // delay gain
  mixer4.gain(2, 0.5);    // launch gain

  delay1.delay(0, 80);
  delay2.delay(0, 100);

  reverb1.inputGain(0.5);
  reverb1.amount(0.15);
  reverb1.time(0.2);

  audio_params.enable(true);
  audio_params.init();

#ifdef PROCESS_COMMANDS
  COMMAND_STREAM.begin(9600);
  command_processor.attach(audio_params);
#endif
}

void AudioManager::loop() {

  // static uint32_t print_time;
  // uint32_t now = millis();
  // if (now - print_time > 1000) {
  //   Serial.printf("Audio Memory: %d, max %d\n",
  //                 AudioMemoryUsage(), AudioMemoryUsageMax());
  //   print_time += 1000;
  // }

#ifdef PROCESS_COMMANDS
  if (command_reader.collect_input()) {
    const char *cmd = command_reader.current_line();
    command_processor.exec_command(cmd);
    command_reader.clear_line();
  }
#endif
}
