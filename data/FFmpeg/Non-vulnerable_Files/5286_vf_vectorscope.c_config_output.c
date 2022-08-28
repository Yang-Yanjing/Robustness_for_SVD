static int config_output(AVFilterLink *outlink)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(outlink->format);
    const int depth = desc->comp[0].depth_minus1 + 1;
    VectorscopeContext *s = outlink->src->priv;
    s->intensity = s->fintensity * ((1 << depth) - 1);
    outlink->h = outlink->w = 1 << depth;
    outlink->sample_aspect_ratio = (AVRational){1,1};
    return 0;
}
