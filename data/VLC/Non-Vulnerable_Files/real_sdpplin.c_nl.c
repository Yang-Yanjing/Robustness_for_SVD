#define BUFLEN 32000
static inline char *nl(char *data) {
  char *nlptr = (data) ? strchr(data,'\n') : NULL;
  return (nlptr) ? nlptr + 1 : NULL;
}
