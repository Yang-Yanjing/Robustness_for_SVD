static av_cold int asink_init(AVFilterContext *ctx, void *opaque)
{
    BufferSinkContext *buf = ctx->priv;
    AVABufferSinkParams *params = opaque;
    int ret;
    if (params) {
        if ((ret = av_opt_set_int_list(buf, "sample_fmts",     params->sample_fmts,  AV_SAMPLE_FMT_NONE, 0)) < 0 ||
            (ret = av_opt_set_int_list(buf, "sample_rates",    params->sample_rates,    -1, 0)) < 0 ||
            (ret = av_opt_set_int_list(buf, "channel_layouts", params->channel_layouts, -1, 0)) < 0 ||
            (ret = av_opt_set_int_list(buf, "channel_counts",  params->channel_counts,  -1, 0)) < 0 ||
            (ret = av_opt_set_int(buf, "all_channel_counts", params->all_channel_counts, 0)) < 0)
            return ret;
    }
    return common_init(ctx);
}
