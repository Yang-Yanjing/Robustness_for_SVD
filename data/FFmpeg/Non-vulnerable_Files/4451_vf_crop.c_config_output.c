static int config_output(AVFilterLink *link)
{
    CropContext *s = link->src->priv;
    link->w = s->w;
    link->h = s->h;
    link->sample_aspect_ratio = s->out_sar;
    return 0;
}
