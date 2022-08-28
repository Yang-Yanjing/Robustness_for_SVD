static int config_output(AVFilterLink *outlink)
{
    PadContext *s = outlink->src->priv;
    outlink->w = s->w;
    outlink->h = s->h;
    return 0;
}
