static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    PadContext *s = ctx->priv;
    int ret;
    double var_values[VARS_NB], res;
    char *expr;
    ff_draw_init(&s->draw, inlink->format, 0);
    ff_draw_color(&s->draw, &s->color, s->rgba_color);
    var_values[VAR_IN_W]  = var_values[VAR_IW] = inlink->w;
    var_values[VAR_IN_H]  = var_values[VAR_IH] = inlink->h;
    var_values[VAR_OUT_W] = var_values[VAR_OW] = NAN;
    var_values[VAR_OUT_H] = var_values[VAR_OH] = NAN;
    var_values[VAR_A]     = (double) inlink->w / inlink->h;
    var_values[VAR_SAR]   = inlink->sample_aspect_ratio.num ?
        (double) inlink->sample_aspect_ratio.num / inlink->sample_aspect_ratio.den : 1;
    var_values[VAR_DAR]   = var_values[VAR_A] * var_values[VAR_SAR];
    var_values[VAR_HSUB]  = 1 << s->draw.hsub_max;
    var_values[VAR_VSUB]  = 1 << s->draw.vsub_max;
    
    av_expr_parse_and_eval(&res, (expr = s->w_expr),
                           var_names, var_values,
                           NULL, NULL, NULL, NULL, NULL, 0, ctx);
    s->w = var_values[VAR_OUT_W] = var_values[VAR_OW] = res;
    if ((ret = av_expr_parse_and_eval(&res, (expr = s->h_expr),
                                      var_names, var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
        goto eval_fail;
    s->h = var_values[VAR_OUT_H] = var_values[VAR_OH] = res;
    
    if ((ret = av_expr_parse_and_eval(&res, (expr = s->w_expr),
                                      var_names, var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
        goto eval_fail;
    s->w = var_values[VAR_OUT_W] = var_values[VAR_OW] = res;
    
    av_expr_parse_and_eval(&res, (expr = s->x_expr),
                           var_names, var_values,
                           NULL, NULL, NULL, NULL, NULL, 0, ctx);
    s->x = var_values[VAR_X] = res;
    if ((ret = av_expr_parse_and_eval(&res, (expr = s->y_expr),
                                      var_names, var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
        goto eval_fail;
    s->y = var_values[VAR_Y] = res;
    
    if ((ret = av_expr_parse_and_eval(&res, (expr = s->x_expr),
                                      var_names, var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
        goto eval_fail;
    s->x = var_values[VAR_X] = res;
    
    if (s->w < 0 || s->h < 0 || s->x < 0 || s->y < 0) {
        av_log(ctx, AV_LOG_ERROR, "Negative values are not acceptable.\n");
        return AVERROR(EINVAL);
    }
    if (!s->w)
        s->w = inlink->w;
    if (!s->h)
        s->h = inlink->h;
    s->w    = ff_draw_round_to_sub(&s->draw, 0, -1, s->w);
    s->h    = ff_draw_round_to_sub(&s->draw, 1, -1, s->h);
    s->x    = ff_draw_round_to_sub(&s->draw, 0, -1, s->x);
    s->y    = ff_draw_round_to_sub(&s->draw, 1, -1, s->y);
    s->in_w = ff_draw_round_to_sub(&s->draw, 0, -1, inlink->w);
    s->in_h = ff_draw_round_to_sub(&s->draw, 1, -1, inlink->h);
    av_log(ctx, AV_LOG_VERBOSE, "w:%d h:%d -> w:%d h:%d x:%d y:%d color:0x%02X%02X%02X%02X\n",
           inlink->w, inlink->h, s->w, s->h, s->x, s->y,
           s->rgba_color[0], s->rgba_color[1], s->rgba_color[2], s->rgba_color[3]);
    if (s->x <  0 || s->y <  0                      ||
        s->w <= 0 || s->h <= 0                      ||
        (unsigned)s->x + (unsigned)inlink->w > s->w ||
        (unsigned)s->y + (unsigned)inlink->h > s->h) {
        av_log(ctx, AV_LOG_ERROR,
               "Input area %d:%d:%d:%d not within the padded area 0:0:%d:%d or zero-sized\n",
               s->x, s->y, s->x + inlink->w, s->y + inlink->h, s->w, s->h);
        return AVERROR(EINVAL);
    }
    return 0;
eval_fail:
    av_log(NULL, AV_LOG_ERROR,
           "Error when evaluating the expression '%s'\n", expr);
    return ret;
}
