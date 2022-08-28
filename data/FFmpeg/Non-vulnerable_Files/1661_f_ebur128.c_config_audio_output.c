static int config_audio_output(AVFilterLink *outlink)
{
    int i;
    AVFilterContext *ctx = outlink->src;
    EBUR128Context *ebur128 = ctx->priv;
    const int nb_channels = av_get_channel_layout_nb_channels(outlink->channel_layout);
#define BACK_MASK (AV_CH_BACK_LEFT    |AV_CH_BACK_CENTER    |AV_CH_BACK_RIGHT| \
                   AV_CH_TOP_BACK_LEFT|AV_CH_TOP_BACK_CENTER|AV_CH_TOP_BACK_RIGHT| \
                   AV_CH_SIDE_LEFT                          |AV_CH_SIDE_RIGHT| \
                   AV_CH_SURROUND_DIRECT_LEFT               |AV_CH_SURROUND_DIRECT_RIGHT)
    ebur128->nb_channels  = nb_channels;
    ebur128->ch_weighting = av_calloc(nb_channels, sizeof(*ebur128->ch_weighting));
    if (!ebur128->ch_weighting)
        return AVERROR(ENOMEM);
    for (i = 0; i < nb_channels; i++) {
        
        const uint16_t chl = av_channel_layout_extract_channel(outlink->channel_layout, i);
        if (chl & (AV_CH_LOW_FREQUENCY|AV_CH_LOW_FREQUENCY_2)) {
            ebur128->ch_weighting[i] = 0;
        } else if (chl & BACK_MASK) {
            ebur128->ch_weighting[i] = 1.41;
        } else {
            ebur128->ch_weighting[i] = 1.0;
        }
        if (!ebur128->ch_weighting[i])
            continue;
        
        ebur128->i400.cache[i]  = av_calloc(I400_BINS,  sizeof(*ebur128->i400.cache[0]));
        ebur128->i3000.cache[i] = av_calloc(I3000_BINS, sizeof(*ebur128->i3000.cache[0]));
        if (!ebur128->i400.cache[i] || !ebur128->i3000.cache[i])
            return AVERROR(ENOMEM);
    }
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
#if CONFIG_SWRESAMPLE
    if (ebur128->peak_mode & PEAK_MODE_TRUE_PEAKS) {
        int ret;
        ebur128->swr_buf    = av_malloc_array(nb_channels, 19200 * sizeof(double));
        ebur128->true_peaks = av_calloc(nb_channels, sizeof(*ebur128->true_peaks));
        ebur128->true_peaks_per_frame = av_calloc(nb_channels, sizeof(*ebur128->true_peaks_per_frame));
        ebur128->swr_ctx    = swr_alloc();
        if (!ebur128->swr_buf || !ebur128->true_peaks ||
            !ebur128->true_peaks_per_frame || !ebur128->swr_ctx)
            return AVERROR(ENOMEM);
        av_opt_set_int(ebur128->swr_ctx, "in_channel_layout",    outlink->channel_layout, 0);
        av_opt_set_int(ebur128->swr_ctx, "in_sample_rate",       outlink->sample_rate, 0);
        av_opt_set_sample_fmt(ebur128->swr_ctx, "in_sample_fmt", outlink->format, 0);
        av_opt_set_int(ebur128->swr_ctx, "out_channel_layout",    outlink->channel_layout, 0);
        av_opt_set_int(ebur128->swr_ctx, "out_sample_rate",       192000, 0);
        av_opt_set_sample_fmt(ebur128->swr_ctx, "out_sample_fmt", outlink->format, 0);
        ret = swr_init(ebur128->swr_ctx);
        if (ret < 0)
            return ret;
    }
#endif
    if (ebur128->peak_mode & PEAK_MODE_SAMPLES_PEAKS) {
        ebur128->sample_peaks = av_calloc(nb_channels, sizeof(*ebur128->sample_peaks));
        if (!ebur128->sample_peaks)
            return AVERROR(ENOMEM);
    }
    return 0;
}
