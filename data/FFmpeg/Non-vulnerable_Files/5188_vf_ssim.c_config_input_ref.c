static int config_input_ref(AVFilterLink *inlink)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    AVFilterContext *ctx  = inlink->dst;
    SSIMContext *s = ctx->priv;
    int sum = 0, i;
    s->nb_components = desc->nb_components;
    if (ctx->inputs[0]->w != ctx->inputs[1]->w ||
        ctx->inputs[0]->h != ctx->inputs[1]->h) {
        av_log(ctx, AV_LOG_ERROR, "Width and height of input videos must be same.\n");
        return AVERROR(EINVAL);
    }
    if (ctx->inputs[0]->format != ctx->inputs[1]->format) {
        av_log(ctx, AV_LOG_ERROR, "Inputs must be of same pixel format.\n");
        return AVERROR(EINVAL);
    }
    s->is_rgb = ff_fill_rgba_map(s->rgba_map, inlink->format) >= 0;
    s->comps[0] = s->is_rgb ? 'R' : 'Y';
    s->comps[1] = s->is_rgb ? 'G' : 'U';
    s->comps[2] = s->is_rgb ? 'B' : 'V';
    s->comps[3] = 'A';
    s->planeheight[1] = s->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    s->planeheight[0] = s->planeheight[3] = inlink->h;
    s->planewidth[1]  = s->planewidth[2]  = FF_CEIL_RSHIFT(inlink->w, desc->log2_chroma_w);
    s->planewidth[0]  = s->planewidth[3]  = inlink->w;
    for (i = 0; i < s->nb_components; i++)
        sum += s->planeheight[i] * s->planewidth[i];
    for (i = 0; i < s->nb_components; i++)
        s->coefs[i] = (double) s->planeheight[i] * s->planewidth[i] / sum;
    s->temp = av_malloc((2 * inlink->w + 12) * sizeof(*s->temp));
    if (!s->temp)
        return AVERROR(ENOMEM);
    s->dsp.ssim_4x4_line = ssim_4x4xn;
    s->dsp.ssim_end_line = ssim_endn;
    if (ARCH_X86)
        ff_ssim_init_x86(&s->dsp);
    return 0;
}
