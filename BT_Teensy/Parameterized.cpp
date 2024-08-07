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

Parameterized::index_option_type
Parameterized::parameter_index(const char *group_member) const {
  for (size_t i = 0; i < count; i++) {
    if (!strcmp(group_member, params[i].group_member)) {
      return i;
    }
  }
  return -1;
}

Parameter::value_type Parameterized::get(const char *group_member) const {
  index_option_type index = parameter_index(group_member);
  if (index < 0) {
    return -1;
  } else {
    return values[index];
  }
}

Parameter::value_type Parameterized::get(size_t index) const {
  return values[index];
}

void Parameterized::init()
{
  for (size_t i = 0; i < count; i++) {
    set(params[i].group_member, params[i].default_);
  }
}

bool
Parameterized::save_value(
  const char *group_member,
  Parameter::value_type new_value)
{
  index_option_type index = parameter_index(group_member);
  if (index < 0) {
    return false;
  }
  const Parameter *p = &params[index];
  if (new_value < p->min || new_value > p->max) {
    return false;
  }
  values[index] = new_value;
  return true;
}