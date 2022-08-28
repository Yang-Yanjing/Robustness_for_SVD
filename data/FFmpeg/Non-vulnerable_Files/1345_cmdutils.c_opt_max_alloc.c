int opt_max_alloc(void *optctx, const char *opt, const char *arg)
{
    char *tail;
    size_t max;
    max = strtol(arg, &tail, 10);
    if (*tail) {
        av_log(NULL, AV_LOG_FATAL, "Invalid max_alloc \"%s\".\n", arg);
        exit_program(1);
    }
    av_max_alloc(max);
    return 0;
}
