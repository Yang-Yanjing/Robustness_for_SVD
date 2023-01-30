static int config_props_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    TransContext *trans = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    const AVPixFmtDescriptor *desc_out = av_pix_fmt_desc_get(outlink->format);
    const AVPixFmtDescriptor *desc_in  = av_pix_fmt_desc_get(inlink->format);
    if (trans->dir&4) {
        av_log(ctx, AV_LOG_WARNING,
               "dir values greater than 3 are deprecated, use the passthrough option instead\n");
        trans->dir &= 3;
        trans->passthrough = TRANSPOSE_PT_TYPE_LANDSCAPE;
    }
    if ((inlink->w >= inlink->h && trans->passthrough == TRANSPOSE_PT_TYPE_LANDSCAPE) ||
        (inlink->w <= inlink->h && trans->passthrough == TRANSPOSE_PT_TYPE_PORTRAIT)) {
        av_log(ctx, AV_LOG_VERBOSE,
               "w:%d h:%d -> w:%d h:%d (passthrough mode)\n",
               inlink->w, inlink->h, inlink->w, inlink->h);
        return 0;
    } else {
        trans->passthrough = TRANSPOSE_PT_TYPE_NONE;
    }
    trans->hsub = desc_in->log2_chroma_w;
    trans->vsub = desc_in->log2_chroma_h;
    av_image_fill_max_pixsteps(trans->pixsteps, NULL, desc_out);
    outlink->w = inlink->h;
    outlink->h = inlink->w;
    if (inlink->sample_aspect_ratio.num)
        outlink->sample_aspect_ratio = av_div_q((AVRational) { 1, 1 },
                                                inlink->sample_aspect_ratio);
    else
        outlink->sample_aspect_ratio = inlink->sample_aspect_ratio;
    av_log(ctx, AV_LOG_VERBOSE,
           "w:%d h:%d dir:%d -> w:%d h:%d rotation:%s vflip:%d\n",
           inlink->w, inlink->h, trans->dir, outlink->w, outlink->h,
           trans->dir == 1 || trans->dir == 3 ? "clockwise" : "counterclockwise",
           trans->dir == 0 || trans->dir == 3);
    return 0;
}
