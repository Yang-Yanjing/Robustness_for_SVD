static int config_input(AVFilterLink *inlink)
{
    SPPContext *s = inlink->dst->priv;
    const int h = FFALIGN(inlink->h + 16, 16);
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    const int bps = desc->comp[0].depth_minus1 + 1;
    av_opt_set_int(s->dct, "bits_per_sample", bps, 0);
    avcodec_dct_init(s->dct);
    if (ARCH_X86)
        ff_spp_init_x86(s);
    s->hsub = desc->log2_chroma_w;
    s->vsub = desc->log2_chroma_h;
    s->temp_linesize = FFALIGN(inlink->w + 16, 16);
    s->temp = av_malloc_array(s->temp_linesize, h * sizeof(*s->temp));
    s->src  = av_malloc_array(s->temp_linesize, h * sizeof(*s->src) * 2);
    if (!s->temp || !s->src)
        return AVERROR(ENOMEM);
    return 0;
}
