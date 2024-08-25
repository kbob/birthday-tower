#define INITIAL_DELAY 1000

#define MIN_DURATION 30
#define MAX_DURATION 200

#define MIN_INTERVAL 100
#define MAX_INTERVAL 3000

#define PIN LED_BUILTIN

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

void setup() {
  Serial.begin(9600);
  // while (!Serial) continue;

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
