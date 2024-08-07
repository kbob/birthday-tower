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
};

class Parameterized {

public:

  typedef long index_option_type; // in a just world, this would be ssize_t.

  virtual ~Parameterized();

  virtual size_t parameter_count() const;
  virtual const Parameter *describe(size_t index) const;
  virtual index_option_type parameter_index(const char *group_member) const;
  virtual Parameter::value_type get(const char *group_member) const;
  virtual Parameter::value_type get(size_t index) const;
  virtual void 
  set(const char *group_member, Parameter::value_type new_value) = 0;

protected:
  Parameterized(const Parameter *ps, Parameter::value_type *vs, size_t ct)
  : params(ps), values(vs), count(ct)
  {}
  
  void init();

  bool save_value(const char *group_member, Parameter::value_type value);

private:
  const Parameter *params;
  Parameter::value_type *values;
  size_t count;
};

#endif /* !PARAMETERIZED_included */
