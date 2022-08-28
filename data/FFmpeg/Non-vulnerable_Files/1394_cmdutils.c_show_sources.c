int show_sources(void *optctx, const char *opt, const char *arg)
{
    AVInputFormat *fmt = NULL;
    char *dev = NULL;
    AVDictionary *opts = NULL;
    int ret = 0;
    int error_level = av_log_get_level();
    av_log_set_level(AV_LOG_ERROR);
    if ((ret = show_sinks_sources_parse_arg(arg, &dev, &opts)) < 0)
        goto fail;
    do {
        fmt = av_input_audio_device_next(fmt);
        if (fmt) {
            if (!strcmp(fmt->name, "lavfi"))
                continue; 
            if (dev && !av_match_name(dev, fmt->name))
                continue;
            print_device_sources(fmt, opts);
        }
    } while (fmt);
    do {
        fmt = av_input_video_device_next(fmt);
        if (fmt) {
            if (dev && !av_match_name(dev, fmt->name))
                continue;
            print_device_sources(fmt, opts);
        }
    } while (fmt);
  fail:
    av_dict_free(&opts);
    av_free(dev);
    av_log_set_level(error_level);
    return ret;
}
