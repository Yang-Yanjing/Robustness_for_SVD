static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    DrawTextContext *s = ctx->priv;
    int ret;
    ff_draw_init(&s->dc, inlink->format, 0);
    ff_draw_color(&s->dc, &s->fontcolor,   s->fontcolor.rgba);
    ff_draw_color(&s->dc, &s->shadowcolor, s->shadowcolor.rgba);
    ff_draw_color(&s->dc, &s->bordercolor, s->bordercolor.rgba);
    ff_draw_color(&s->dc, &s->boxcolor,    s->boxcolor.rgba);
    s->var_values[VAR_w]     = s->var_values[VAR_W]     = s->var_values[VAR_MAIN_W] = inlink->w;
    s->var_values[VAR_h]     = s->var_values[VAR_H]     = s->var_values[VAR_MAIN_H] = inlink->h;
    s->var_values[VAR_SAR]   = inlink->sample_aspect_ratio.num ? av_q2d(inlink->sample_aspect_ratio) : 1;
    s->var_values[VAR_DAR]   = (double)inlink->w / inlink->h * s->var_values[VAR_SAR];
    s->var_values[VAR_HSUB]  = 1 << s->dc.hsub_max;
    s->var_values[VAR_VSUB]  = 1 << s->dc.vsub_max;
    s->var_values[VAR_X]     = NAN;
    s->var_values[VAR_Y]     = NAN;
    s->var_values[VAR_T]     = NAN;
    av_lfg_init(&s->prng, av_get_random_seed());
    av_expr_free(s->x_pexpr);
    av_expr_free(s->y_pexpr);
    s->x_pexpr = s->y_pexpr = NULL;
    if ((ret = av_expr_parse(&s->x_pexpr, s->x_expr, var_names,
                             NULL, NULL, fun2_names, fun2, 0, ctx)) < 0 ||
        (ret = av_expr_parse(&s->y_pexpr, s->y_expr, var_names,
                             NULL, NULL, fun2_names, fun2, 0, ctx)) < 0 ||
        (ret = av_expr_parse(&s->a_pexpr, s->a_expr, var_names,
                             NULL, NULL, fun2_names, fun2, 0, ctx)) < 0)
        return AVERROR(EINVAL);
    return 0;
}
