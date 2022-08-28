static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    PP7Context *pp7 = ctx->priv;
    const int h = FFALIGN(inlink->h + 16, 16);
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    pp7->hsub = desc->log2_chroma_w;
    pp7->vsub = desc->log2_chroma_h;
    pp7->temp_stride = FFALIGN(inlink->w + 16, 16);
    pp7->src = av_malloc_array(pp7->temp_stride,  (h + 8) * sizeof(uint8_t));
    if (!pp7->src)
        return AVERROR(ENOMEM);
    init_thres2(pp7);
    switch (pp7->mode) {
        case 0: pp7->requantize = hardthresh_c; break;
        case 1: pp7->requantize = softthresh_c; break;
        default:
        case 2: pp7->requantize = mediumthresh_c; break;
    }
    pp7->dctB = dctB_c;
    if (ARCH_X86)
        ff_pp7_init_x86(pp7);
    return 0;
}
