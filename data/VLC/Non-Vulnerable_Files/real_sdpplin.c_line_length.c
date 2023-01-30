}
static inline int line_length(char * data) {
  char const * p = nl(data);
  if (p) {
    return p - data - 1;
  }
  return strlen(data);
}
}
static inline int line_length(char * data) {
  char const * p = nl(data);
  if (p) {
    return p - data - 1;
  }
  return strlen(data);
}
