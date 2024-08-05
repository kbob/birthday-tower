#include "Trace.h"

#include <Arduino.h>

#define MAX_TRACES 300
#define MAX_MSG_LEN (64 - sizeof (uint32_t))

struct trace_entry {
  uint32_t usec;
  char msg[MAX_MSG_LEN];
};
trace_entry trace_buffer[MAX_TRACES];
size_t trace_count;

void Trace::trace_message(const char *msg) {
  uint32_t now = micros();
  if (trace_count >= MAX_TRACES)
    return;

  trace_entry *p = &trace_buffer[trace_count++];
  p->usec = now;
  strncpy(p->msg, msg, sizeof p->msg - 1);
  p->msg[MAX_MSG_LEN - 1] = '\0';
}

void Trace::tracef(const char *fmt, ...)
{
  if (trace_count >= MAX_TRACES)
    return;

  trace_entry *p = &trace_buffer[trace_count++];
  p->usec = micros();
  va_list ap;
  va_start(ap, fmt);
  (void) vsnprintf(p->msg, sizeof p->msg, fmt, ap);
  va_end(ap);
}

void Trace::print_all() {
  Serial.printf("Traces\n");
  for (size_t i = 0; i < trace_count; i++) {
    const trace_entry *p = &trace_buffer[i];
    Serial.printf("%8d: %s\n", p->usec, p->msg);
  }
  Serial.printf("\n");
  if (trace_count == MAX_TRACES) {
      Serial.printf("TRACE BUFFER IS FULL\n");
  }
}