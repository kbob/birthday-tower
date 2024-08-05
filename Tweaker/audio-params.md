commands
  * `l` - list variables
  * `g GROUP.VAR` - get variable
  * `s GROUP.VAR VALUE` - set variable
  * `h`, `?` - help

list format

    name.subname type min max default units
What about units?

types:

 * i - int
 * f - linear float
 * e - exponential float

get/set response
group.member value
value is whatever `strtof` or `strtol` can parse.

On the Teensy:

    enum scale {
        LINEAR,
        LOG,
        EXP,
    };
    // also square, cube, sqrt, cube_root

    // could separate into param_desc and param_value...
    struct int_param {
        const char *group_member;
        const int min;
        const int max;
        const int default;
        const char *units;
        int value;
        bool changed;
    };
    struct float_param {
        const char *group_member;
        const enum scale scale;
        const float min;
        const float max;
        const float default;
        const char *units;
        float value;
        bool changed;
    };

    int_param *get_int_param(const char *group_member) {
        for (const int_param *p = int_params; p < (&int_params)[1]; p++) {
            if !strcmp(group_member, p->group_member)
                return p;
        }
        return NULL;
    }