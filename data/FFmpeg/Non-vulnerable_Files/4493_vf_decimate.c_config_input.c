static int config_input(AVFilterLink *inlink)
{
    int max_value;
    AVFilterContext *ctx = inlink->dst;
    DecimateContext *dm = ctx->priv;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(inlink->format);
    const int w = inlink->w;
    const int h = inlink->h;
    dm->hsub      = pix_desc->log2_chroma_w;
    dm->vsub      = pix_desc->log2_chroma_h;
    dm->depth     = pix_desc->comp[0].depth_minus1 + 1;
    max_value     = (1 << dm->depth) - 1;
    dm->scthresh  = (int64_t)(((int64_t)max_value *          w * h          * dm->scthresh_flt)  / 100);
    dm->dupthresh = (int64_t)(((int64_t)max_value * dm->blockx * dm->blocky * dm->dupthresh_flt) / 100);
    dm->nxblocks  = (w + dm->blockx/2 - 1) / (dm->blockx/2);
    dm->nyblocks  = (h + dm->blocky/2 - 1) / (dm->blocky/2);
    dm->bdiffsize = dm->nxblocks * dm->nyblocks;
    dm->bdiffs    = av_malloc_array(dm->bdiffsize, sizeof(*dm->bdiffs));
    dm->queue     = av_calloc(dm->cycle, sizeof(*dm->queue));
    if (!dm->bdiffs || !dm->queue)
        return AVERROR(ENOMEM);
    if (dm->ppsrc) {
        dm->clean_src = av_calloc(dm->cycle, sizeof(*dm->clean_src));
        if (!dm->clean_src)
            return AVERROR(ENOMEM);
    }
    return 0;
}
