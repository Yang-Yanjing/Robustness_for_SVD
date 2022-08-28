static av_cold int init(AVFilterContext *ctx)
{
    GradFunContext *s = ctx->priv;
    s->thresh  = (1 << 15) / s->strength;
    s->radius  = av_clip((s->radius + 1) & ~1, 4, 32);
    s->blur_line   = ff_gradfun_blur_line_c;
    s->filter_line = ff_gradfun_filter_line_c;
    if (ARCH_X86)
        ff_gradfun_init_x86(s);
    av_log(ctx, AV_LOG_VERBOSE, "threshold:%.2f radius:%d\n", s->strength, s->radius);
    return 0;
}
