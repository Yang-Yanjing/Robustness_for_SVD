static int config_output(AVFilterLink *outlink)
{
    DrawGraphContext *s = outlink->src->priv;
    outlink->w = s->w;
    outlink->h = s->h;
    outlink->sample_aspect_ratio = (AVRational){1,1};
    return 0;
}
