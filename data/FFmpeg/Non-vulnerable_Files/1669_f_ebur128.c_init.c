static av_cold int init(AVFilterContext *ctx)
{
    EBUR128Context *ebur128 = ctx->priv;
    AVFilterPad pad;
    if (ebur128->loglevel != AV_LOG_INFO &&
        ebur128->loglevel != AV_LOG_VERBOSE) {
        if (ebur128->do_video || ebur128->metadata)
            ebur128->loglevel = AV_LOG_VERBOSE;
        else
            ebur128->loglevel = AV_LOG_INFO;
    }
    if (!CONFIG_SWRESAMPLE && (ebur128->peak_mode & PEAK_MODE_TRUE_PEAKS)) {
        av_log(ctx, AV_LOG_ERROR,
               "True-peak mode requires libswresample to be performed\n");
        return AVERROR(EINVAL);
    }
    
    
    ebur128->scale_range = 3 * ebur128->meter;
    ebur128->i400.histogram  = get_histogram();
    ebur128->i3000.histogram = get_histogram();
    if (!ebur128->i400.histogram || !ebur128->i3000.histogram)
        return AVERROR(ENOMEM);
    ebur128->integrated_loudness = ABS_THRES;
    ebur128->loudness_range = 0;
    
    if (ebur128->do_video) {
        pad = (AVFilterPad){
            .name         = av_strdup("out0"),
            .type         = AVMEDIA_TYPE_VIDEO,
            .config_props = config_video_output,
        };
        if (!pad.name)
            return AVERROR(ENOMEM);
        ff_insert_outpad(ctx, 0, &pad);
    }
    pad = (AVFilterPad){
        .name         = av_asprintf("out%d", ebur128->do_video),
        .type         = AVMEDIA_TYPE_AUDIO,
        .config_props = config_audio_output,
    };
    if (!pad.name)
        return AVERROR(ENOMEM);
    ff_insert_outpad(ctx, ebur128->do_video, &pad);
    
    av_log(ctx, AV_LOG_VERBOSE, "EBU +%d scale\n", ebur128->meter);
    return 0;
}
