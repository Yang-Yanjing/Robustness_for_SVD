static int config_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    FliteContext *flite = ctx->priv;
    outlink->sample_rate = flite->wave->sample_rate;
    outlink->time_base = (AVRational){1, flite->wave->sample_rate};
    av_log(ctx, AV_LOG_VERBOSE, "voice:%s fmt:%s sample_rate:%d\n",
           flite->voice_str,
           av_get_sample_fmt_name(outlink->format), outlink->sample_rate);
    return 0;
}
