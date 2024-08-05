#include "Parameterized.h"

#include <string.h>

Parameterized::~Parameterized() {}

size_t Parameterized::parameter_count() const {
  return count;
}

const Parameter *Parameterized::describe(size_t index) const {
  if (index >= count) {
    return NULL;
  }
  return &params[index];
}

Parameter::value_type Parameterized::get(const char *group_member) const {
  for (size_t i = 0; i < count; i++) {
    if (!strcmp(group_member, params[i].group_member)) {
      return values[i];
    }
  }
  return -1;
}

#include <Arduino.h>

void Parameterized::init()
{
  for (size_t i = 0; i < count; i++) {
    // set(params[i].group_member, params[i].default_);
  }
}

bool
Parameterized::save_value(
  const char *group_member,
  Parameter::value_type new_value)
{
  for (size_t i = 0; i < count; i++) {
    const Parameter *p = &params[i];
    if (!strcmp(group_member, p->group_member)) {
      if (new_value < p->min || new_value > p->max) {
        return false;
      }
      values[i] = new_value;
      return true;
    }
  }
  return false;
}