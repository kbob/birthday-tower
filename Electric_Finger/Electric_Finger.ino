#define INITIAL_DELAY 1000

#define MIN_DURATION 5
#define MAX_DURATION 400

#define MIN_INTERVAL 100
#define MAX_INTERVAL 3000

#define PIN LED_BUILTIN

#define MIN_BPM 60
#define MAX_BPM 180

#define MIN_PHRASE_SPACE 2000
#define MAX_PHRASE_SPACE 12000

#define NOTE_DENSITY 0.6

uint32_t bpm_to_msec(float bpm) {
  // 60 sec/min * 1000 msec/sec / bpm beat/sec => msec/beat
  return 60000.0f / bpm;
}

float msec_to_bpm(uint32_t msec) {
  // 1000 msec/sec * 60 sec/min / n msec/beat => beat/min
  return 60000.0f / msec;
}

struct Event {
  enum Type { ON, OFF, NO_EVENT };

  Type what;
  uint8_t beat;  // 0..7
  uint32_t time; // msec relative to phrase start

  Event(Type what, uint8_t beat, uint32_t time)
  : what(what),
    beat(beat),
    time(time)
  {}
};
Event NO_EVENT(Event::NO_EVENT, 0, 0);

struct Phrase {
  enum {
    BEATS_PER_PHRASE = 8,
  };

  Event next_event(uint32_t now) {
    // uint32_t tempo_msec = bpm_to_msec(tempo);
    uint32_t tempo_msec = 0;

    for (int beat = 0; beat < BEATS_PER_PHRASE; beat++) {
      uint32_t beat_time = beat * tempo_msec;
      if (!durations[beat]) {
        continue;
      }
      uint32_t on_time = beat_time;
      if (on_time >= now) {
        return Event(Event::ON, beat, on_time);
      }
      uint32_t off_time = on_time + durations[beat];
      if (off_time >= now) {
        return Event(Event::OFF, beat, off_time);
      }
    }
    return NO_EVENT;
  }

  float tempo;
  uint32_t durations[BEATS_PER_PHRASE];
};

void gen_phrase(struct Phrase& p) {
  p.tempo = random(MIN_BPM, MAX_BPM);
  for (size_t i = 0; i < Phrase::BEATS_PER_PHRASE; i++) {
    uint16_t dur = 0;
    if (random(1000) / 1000.0f < NOTE_DENSITY) {
      dur = random(MIN_DURATION, MAX_DURATION);
    }
    p.durations[i] = dur;
  }
}






int state;
uint32_t next_time;


unsigned long collect_entropy() {
  static const int entropy_pins[] = {
    A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10,
    A11, A12, A13, A14, A15, A16, A17, A18, A19, A20,
  };
  static const size_t entropy_pin_count = (&entropy_pins)[1] - entropy_pins;

  unsigned long seed = micros();
  analogReadResolution(16);
  for (int round = 0; round < 10; round++) {
    for (size_t i = 0; i < entropy_pin_count; i++) {
      seed += analogRead(entropy_pins[i]);
    }
  }
  return seed;
}

// global variables
// phrase start time


void setup() {
  Serial.begin(9600);
  while (!Serial) continue;

  unsigned long seed = collect_entropy();
  randomSeed(seed);
  Serial.printf("seed = %d\n", seed);

  state = HIGH;
  digitalWrite(PIN, state);
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, state);
  next_time = millis() + INITIAL_DELAY;
}

void loop() {
  if (millis() < next_time)
    return;

  // Serial.println("Oof");
  if (state == HIGH) {
    state = LOW;
    next_time += random(MIN_DURATION, MAX_DURATION);
  } else {
    state = HIGH;
    next_time += random(MIN_INTERVAL, MAX_INTERVAL);
  }
  digitalWrite(PIN, state);
}
