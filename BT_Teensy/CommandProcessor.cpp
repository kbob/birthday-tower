#include "CommandProcessor.h"

#include <Arduino.h>

void CommandProcessor::attach(Parameterized& thing) {
    if (thing_count < MAX_PARAMETERIZED_THINGS) {
        things[thing_count++] = &thing;
    }
}

// There are three commands.
//   list
//   get PARAM
//   set PARAM VALUE
void CommandProcessor::exec_command(const char *cmd) {
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
  out.printf("Unknown command: \"%.*s\"\n", len, cmd);
}

void CommandProcessor::exec_list_command(const char *cmd) {
  for (size_t i = 0; i < thing_count; i++) {
    const Parameterized *ptp = things[i];
    for (size_t j = 0; j < ptp->parameter_count(); j++) {
      const Parameter *p = ptp->describe(j);
      const char *units = p->units;
      if (!units)
        units = "";
      out.printf("%s %d %d %d %s\n",
        p->group_member, p->min, p->max, p->default_, units);
    }
  }
  out.printf("-\n");
}

void CommandProcessor::exec_get_command(const char *cmd) {
  char group_member[40];
  int n = sscanf(cmd, "get %39s\n", group_member);
  if (n != 1) {
    out.printf("get: missing parameter name\n");
    return;
  }
  for (size_t i = 0; i < thing_count; i++) {
    Parameterized *ptp = things[i];
    for (size_t j = 0; j < ptp->parameter_count(); j++) {
      const Parameter *p = ptp->describe(j);
      if (!strcmp(p->group_member, group_member)) {
        out.printf("%s=%d\n", group_member, ptp->get(group_member));
        return;
      }
    }
  }
  out.printf("Parameter %s not found.\n", group_member);
}

void CommandProcessor::exec_set_command(const char *cmd) {
  char group_member[40];
  int new_value;
  int n = sscanf(cmd, "set %39s %d\n", group_member, &new_value);
  if (n != 2) {
    out.printf("get: missing parameter name or value\n");
    return;
  }
  for (size_t i = 0; i < thing_count; i++) {
    Parameterized *ptp = things[i];
    for (size_t j = 0; j < ptp->parameter_count(); j++) {
      const Parameter *p = ptp->describe(j);
      if (!strcmp(p->group_member, group_member)) {
        if (new_value < p->min || new_value > p->max) {
          out.printf("%s: value %g is out of range.",
                                group_member, new_value);
        } else {
          ptp->set(group_member, new_value);
          out.printf("OK\n");
        }
        return;
      }
    }
  }
  out.printf("Parameter %s not found.\n", group_member);
}
