#ifndef COMMAND_PROCESSOR_included
#define COMMAND_PROCESSOR_included

#include "Parameterized.h"

class Print;

class CommandProcessor {

public:
    enum {
        MAX_PARAMETERIZED_THINGS = 10,
    };

    CommandProcessor(Print& out)
    : thing_count(0),
      out(out)
    {}

    void attach(Parameterized& thing);

    void exec_command(const char *cmd);

private:
    void exec_list_command(const char *cmd);
    void exec_get_command(const char *cmd);
    void exec_set_command(const char *cmd);

    Parameterized *things[MAX_PARAMETERIZED_THINGS];
    size_t thing_count;
    Print& out;
};

#endif /* !COMMAND_PROCESSOR_included */
