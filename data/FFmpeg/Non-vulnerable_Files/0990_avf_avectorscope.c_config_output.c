static int config_output(AVFilterLink *outlink)
{
    AudioVectorScopeContext *s = outlink->src->priv;
    outlink->w = s->w;
    outlink->h = s->h;
    outlink->sample_aspect_ratio = (AVRational){1,1};
    outlink->frame_rate = s->frame_rate;
    s->hw = s->w / 2;
    s->hh = s->h / 2;
    return 0;
}
