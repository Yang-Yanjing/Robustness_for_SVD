void show_help_default(const char *opt, const char *arg)
{
    av_log_set_callback(log_callback_help);
    show_usage();
    show_help_options(options, "Main options:", 0, 0, 0);
    printf("\n");
    show_help_children(avformat_get_class(), AV_OPT_FLAG_DECODING_PARAM);
    show_help_children(avcodec_get_class(), AV_OPT_FLAG_DECODING_PARAM);
}
