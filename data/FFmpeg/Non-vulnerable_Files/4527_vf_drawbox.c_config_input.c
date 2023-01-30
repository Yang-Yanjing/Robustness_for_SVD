static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    DrawBoxContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    double var_values[VARS_NB], res;
    char *expr;
    int ret;
    int i;
    s->hsub = desc->log2_chroma_w;
    s->vsub = desc->log2_chroma_h;
    var_values[VAR_IN_H] = var_values[VAR_IH] = inlink->h;
    var_values[VAR_IN_W] = var_values[VAR_IW] = inlink->w;
    var_values[VAR_SAR]  = inlink->sample_aspect_ratio.num ? av_q2d(inlink->sample_aspect_ratio) : 1;
    var_values[VAR_DAR]  = (double)inlink->w / inlink->h * var_values[VAR_SAR];
    var_values[VAR_HSUB] = s->hsub;
    var_values[VAR_VSUB] = s->vsub;
    var_values[VAR_X] = NAN;
    var_values[VAR_Y] = NAN;
    var_values[VAR_H] = NAN;
    var_values[VAR_W] = NAN;
    var_values[VAR_T] = NAN;
    for (i = 0; i <= NUM_EXPR_EVALS; i++) {
        
        var_values[VAR_MAX] = inlink->w;
        if ((ret = av_expr_parse_and_eval(&res, (expr = s->x_expr),
                                          var_names, var_values,
                                          NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0 && i == NUM_EXPR_EVALS)
            goto fail;
        s->x = var_values[VAR_X] = res;
        var_values[VAR_MAX] = inlink->h;
        if ((ret = av_expr_parse_and_eval(&res, (expr = s->y_expr),
                                          var_names, var_values,
                                          NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0 && i == NUM_EXPR_EVALS)
            goto fail;
        s->y = var_values[VAR_Y] = res;
        var_values[VAR_MAX] = inlink->w - s->x;
        if ((ret = av_expr_parse_and_eval(&res, (expr = s->w_expr),
                                          var_names, var_values,
                                          NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0 && i == NUM_EXPR_EVALS)
            goto fail;
        s->w = var_values[VAR_W] = res;
        var_values[VAR_MAX] = inlink->h - s->y;
        if ((ret = av_expr_parse_and_eval(&res, (expr = s->h_expr),
                                          var_names, var_values,
                                          NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0 && i == NUM_EXPR_EVALS)
            goto fail;
        s->h = var_values[VAR_H] = res;
        var_values[VAR_MAX] = INT_MAX;
        if ((ret = av_expr_parse_and_eval(&res, (expr = s->t_expr),
                                          var_names, var_values,
                                          NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0 && i == NUM_EXPR_EVALS)
            goto fail;
        s->thickness = var_values[VAR_T] = res;
    }
    
    s->w = (s->w > 0) ? s->w : inlink->w;
    s->h = (s->h > 0) ? s->h : inlink->h;
    
    if (s->w <  0 || s->h <  0) {
        av_log(ctx, AV_LOG_ERROR, "Size values less than 0 are not acceptable.\n");
        return AVERROR(EINVAL);
    }
    av_log(ctx, AV_LOG_VERBOSE, "x:%d y:%d w:%d h:%d color:0x%02X%02X%02X%02X\n",
           s->x, s->y, s->w, s->h,
           s->yuv_color[Y], s->yuv_color[U], s->yuv_color[V], s->yuv_color[A]);
    return 0;
fail:
    av_log(ctx, AV_LOG_ERROR,
           "Error when evaluating the expression '%s'.\n",
           expr);
    return ret;
}
