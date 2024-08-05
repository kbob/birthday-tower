#ifndef TRACE_included
#define TRACE_included

class Trace {

public:
    static void tracef(const char *format, ...);
    static void trace_message(const char *);
    static void print_all();
};

#endif /* !TRACE_included */
