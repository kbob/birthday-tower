# idea

a note has a time and a duration.
a phrase has a sequence of notes.
  - times have some relation, possibly powers of two.
  - theme has a tempo.

a variation changes a phrase.
  - add/remove notes
  - change notes' durations
  - double/half time.

phrases are repeated with variations
  - at a regular interval
  - at irregular intervals

phrases are archived and replayed later.

space (silence) separates phrases/variations.

```
#define MIN_BPM 60
#define MAX_BPM 180

#define MIN_PHRASE_SPACE 2000
#define MAX_PHRASE_SPACE 12000

#define NOTE_DENSITY 0.6

>uint32_t bpm_to_msec(float bpm) {
    // 60 sec/min * 1000 msec/sec / bpm beat/sec => msec/beat
    return 60000.0f / bpm;
}

float msec_to_bpm(uint32_t msec) {
    // 1000 msec/sec * 60 sec/min / n msec/beat => beat/min
    return 60000.0f / msec;
}

class Event {
    enum { ON, OFF } what;
    uint8_t beat;  // 0..7
    uint32_t time; // msec
};

struct Phrase {
    enum {
        BEATS_PER_PHRASE = 8;
    };

    uint32_t next_event_time(uint32_t now);

    float tempo;  // bpm?
    uint16_t durations[8];

};

void gen_phrase(Phrase& p) {
    p.tempo = random(MIN_TEMPO, MAX_TEMPO);
    for (size_t i = 0; i < Phrase::BEATS_PER_PHRASE; i++) {
        uint16_t dur = 0;
        if (random(1000) / 1000.0f < NOTE_DENSITY) {
            dur = random(MIN_DURATION, MAX_DURATION);
        }
        p.durations[i] = dur;
    }
}



```