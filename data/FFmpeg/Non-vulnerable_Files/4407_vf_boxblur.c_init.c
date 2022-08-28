static av_cold int init(AVFilterContext *ctx)
{
    BoxBlurContext *s = ctx->priv;
    if (!s->luma_param.radius_expr) {
        av_log(ctx, AV_LOG_ERROR, "Luma radius expression is not set.\n");
        return AVERROR(EINVAL);
    }
    
    if (!s->chroma_param.radius_expr) {
        s->chroma_param.radius_expr = av_strdup(s->luma_param.radius_expr);
        if (!s->chroma_param.radius_expr)
            return AVERROR(ENOMEM);
    }
    if (s->chroma_param.power < 0)
        s->chroma_param.power = s->luma_param.power;
    if (!s->alpha_param.radius_expr) {
        s->alpha_param.radius_expr = av_strdup(s->luma_param.radius_expr);
        if (!s->alpha_param.radius_expr)
            return AVERROR(ENOMEM);
    }
    if (s->alpha_param.power < 0)
        s->alpha_param.power = s->luma_param.power;
    return 0;
}
