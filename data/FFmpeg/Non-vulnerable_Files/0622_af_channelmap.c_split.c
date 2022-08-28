AVFILTER_DEFINE_CLASS(channelmap);
static char* split(char *message, char delim) {
    char *next = strchr(message, delim);
    if (next)
      *next++ = '\0';
    return next;
}
