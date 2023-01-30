static int config_props(AVFilterLink *outlink)
{
    ANullContext *null = outlink->src->priv;
    char buf[128];
    av_get_channel_layout_string(buf, sizeof(buf), 0, null->channel_layout);
    av_log(outlink->src, AV_LOG_VERBOSE,
           "sample_rate:%d channel_layout:'%s' nb_samples:%d\n",
           null->sample_rate, buf, null->nb_samples);
    return 0;
}
