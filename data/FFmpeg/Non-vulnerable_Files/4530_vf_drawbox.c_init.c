static const int NUM_EXPR_EVALS = 5;
static av_cold int init(AVFilterContext *ctx)
{
    DrawBoxContext *s = ctx->priv;
    uint8_t rgba_color[4];
    if (!strcmp(s->color_str, "invert"))
        s->invert_color = 1;
    else if (av_parse_color(rgba_color, s->color_str, -1, ctx) < 0)
        return AVERROR(EINVAL);
    if (!s->invert_color) {
        s->yuv_color[Y] = RGB_TO_Y_CCIR(rgba_color[0], rgba_color[1], rgba_color[2]);
        s->yuv_color[U] = RGB_TO_U_CCIR(rgba_color[0], rgba_color[1], rgba_color[2], 0);
        s->yuv_color[V] = RGB_TO_V_CCIR(rgba_color[0], rgba_color[1], rgba_color[2], 0);
        s->yuv_color[A] = rgba_color[3];
    }
    return 0;
}
