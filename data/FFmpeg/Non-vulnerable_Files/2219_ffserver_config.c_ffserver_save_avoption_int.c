static int ffserver_save_avoption_int(const char *opt, int64_t arg,
                                      int type, FFServerConfig *config)
{
    char buf[22];
    snprintf(buf, sizeof(buf), "%"PRId64, arg);
    return ffserver_save_avoption(opt, buf, type, config);
}
