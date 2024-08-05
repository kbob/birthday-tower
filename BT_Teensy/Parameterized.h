#include <stddef.h>

#ifndef PARAMETERIZED_included
#define PARAMETERIZED_included

struct Parameter {
  typedef int value_type;

  const char *group_member;
  const value_type min;
  const value_type max;
  const value_type default_;
  const char *units;

  Parameter(const char *gm, value_type mn, value_type mx, value_type df, const char *un = "")
  : group_member(gm),
    min(mn),
    max(mx),
    default_(df),
    units(un)
  {}
};

class Parameterized {

public:

  virtual ~Parameterized();

  virtual size_t parameter_count() const;
  virtual const Parameter *describe(size_t index) const;
  virtual Parameter::value_type get(const char *group_member) const;
  virtual void set(const char *group_member, Parameter::value_type new_value);

protected:
  Parameterized(const Parameter *params, Parameter::value_type *values, size_t count)
  : params(params), values(values), count(count)
  {}
  
  void init();

  bool save_value(const char *group_member, Parameter::value_type value);

private:
  const Parameter *params;
  Parameter::value_type *values;
  size_t count;
};

#endif /* !PARAMETERIZED_included */
