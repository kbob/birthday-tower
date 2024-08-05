class StreamReader {
  
  enum {
    LINE_BUFFER_SIZE = 100,
  };

public:
  StreamReader(Stream& in_stream)
  : in(in_stream), length(0), discard_rest_of_line(false)
  {}

  operator bool () const {  // true if a complete line is available
    return length > 0 && line_buffer[length - 1] == '\n';
  }

  const char *current_line() const {
    return line_buffer;
  }
  void clear_line() {
    length = 0;
    line_buffer[length] = '\0';
  }
  bool collect_input() {
    while (!*this && in.available()) {
      int c = in.read();
      if (c == '\r')
        c = '\n';
      if (discard_rest_of_line) {
        if (c == '\n')
          discard_rest_of_line = false;
        continue;
      }
      line_buffer[length++] = c;
      line_buffer[length] = '\0';
      if (c != '\n' && length == LINE_BUFFER_SIZE - 2) {
        line_buffer[length++] = '\n';
        line_buffer[length] = '\0';
        discard_rest_of_line = true;
      }
    }
    return bool(*this);
  }

private:
  Stream& in;
  size_t length;
  bool discard_rest_of_line;
  char line_buffer[LINE_BUFFER_SIZE];
};
