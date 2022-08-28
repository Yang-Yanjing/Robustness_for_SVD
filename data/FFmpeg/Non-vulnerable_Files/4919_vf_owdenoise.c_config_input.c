static int config_input(AVFilterLink *inlink)
{
    int i, j;
    OWDenoiseContext *s = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    const int h = FFALIGN(inlink->h, 16);
    s->hsub = desc->log2_chroma_w;
    s->vsub = desc->log2_chroma_h;
    s->linesize = FFALIGN(inlink->w, 16);
    for (j = 0; j < 4; j++) {
        for (i = 0; i <= s->depth; i++) {
            s->plane[i][j] = av_malloc_array(s->linesize, h * sizeof(s->plane[0][0][0]));
            if (!s->plane[i][j])
                return AVERROR(ENOMEM);
        }
    }
    return 0;
}
