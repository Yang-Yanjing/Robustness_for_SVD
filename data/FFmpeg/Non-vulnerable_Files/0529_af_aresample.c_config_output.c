static int config_output(AVFilterLink *outlink)
{
    int ret;
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = ctx->inputs[0];
    AResampleContext *aresample = ctx->priv;
    int64_t out_rate, out_layout;
    enum AVSampleFormat out_format;
    char inchl_buf[128], outchl_buf[128];
    aresample->swr = swr_alloc_set_opts(aresample->swr,
                                        outlink->channel_layout, outlink->format, outlink->sample_rate,
                                        inlink->channel_layout, inlink->format, inlink->sample_rate,
                                        0, ctx);
    if (!aresample->swr)
        return AVERROR(ENOMEM);
    if (!inlink->channel_layout)
        av_opt_set_int(aresample->swr, "ich", inlink->channels, 0);
    if (!outlink->channel_layout)
        av_opt_set_int(aresample->swr, "och", outlink->channels, 0);
    ret = swr_init(aresample->swr);
    if (ret < 0)
        return ret;
    av_opt_get_int(aresample->swr, "osr", 0, &out_rate);
    av_opt_get_int(aresample->swr, "ocl", 0, &out_layout);
    av_opt_get_sample_fmt(aresample->swr, "osf", 0, &out_format);
    outlink->time_base = (AVRational) {1, out_rate};
    av_assert0(outlink->sample_rate == out_rate);
    av_assert0(outlink->channel_layout == out_layout || !outlink->channel_layout);
    av_assert0(outlink->format == out_format);
    aresample->ratio = (double)outlink->sample_rate / inlink->sample_rate;
    av_get_channel_layout_string(inchl_buf,  sizeof(inchl_buf),  inlink ->channels, inlink ->channel_layout);
    av_get_channel_layout_string(outchl_buf, sizeof(outchl_buf), outlink->channels, outlink->channel_layout);
    av_log(ctx, AV_LOG_VERBOSE, "ch:%d chl:%s fmt:%s r:%dHz -> ch:%d chl:%s fmt:%s r:%dHz\n",
           inlink ->channels, inchl_buf,  av_get_sample_fmt_name(inlink->format),  inlink->sample_rate,
           outlink->channels, outchl_buf, av_get_sample_fmt_name(outlink->format), outlink->sample_rate);
    return 0;
}
