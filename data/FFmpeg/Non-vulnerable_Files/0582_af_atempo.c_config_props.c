static int config_props(AVFilterLink *inlink)
{
    AVFilterContext  *ctx = inlink->dst;
    ATempoContext *atempo = ctx->priv;
    enum AVSampleFormat format = inlink->format;
    int sample_rate = (int)inlink->sample_rate;
    int channels = av_get_channel_layout_nb_channels(inlink->channel_layout);
    ctx->outputs[0]->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    return yae_reset(atempo, format, sample_rate, channels);
}
