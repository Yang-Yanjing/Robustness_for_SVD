int show_version(void *optctx, const char *opt, const char *arg)
{
    av_log_set_callback(log_callback_help);
    print_program_info (SHOW_COPYRIGHT, AV_LOG_INFO);
    print_all_libs_info(SHOW_VERSION, AV_LOG_INFO);
    return 0;
}
