static void show_help_demuxer(const char *name)
{
    const AVInputFormat *fmt = av_find_input_format(name);
    if (!fmt) {
        av_log(NULL, AV_LOG_ERROR, "Unknown format '%s'.\n", name);
        return;
    }
    printf("Demuxer %s [%s]:\n", fmt->name, fmt->long_name);
    if (fmt->extensions)
        printf("    Common extensions: %s.\n", fmt->extensions);
    if (fmt->priv_class)
        show_help_children(fmt->priv_class, AV_OPT_FLAG_DECODING_PARAM);
}
