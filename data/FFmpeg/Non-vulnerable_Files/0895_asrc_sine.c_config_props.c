static av_cold int config_props(AVFilterLink *outlink)
{
    SineContext *sine = outlink->src->priv;
    sine->duration = av_rescale(sine->duration, sine->sample_rate, AV_TIME_BASE);
    return 0;
}
