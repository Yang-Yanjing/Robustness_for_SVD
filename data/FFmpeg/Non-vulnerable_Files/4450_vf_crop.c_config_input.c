static int config_input(AVFilterLink *link)
{
    AVFilterContext *ctx = link->dst;
    CropContext *s = ctx->priv;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(link->format);
    int ret;
    const char *expr;
    double res;
    s->var_values[VAR_IN_W]  = s->var_values[VAR_IW] = ctx->inputs[0]->w;
    s->var_values[VAR_IN_H]  = s->var_values[VAR_IH] = ctx->inputs[0]->h;
    s->var_values[VAR_A]     = (float) link->w / link->h;
    s->var_values[VAR_SAR]   = link->sample_aspect_ratio.num ? av_q2d(link->sample_aspect_ratio) : 1;
    s->var_values[VAR_DAR]   = s->var_values[VAR_A] * s->var_values[VAR_SAR];
    s->var_values[VAR_HSUB]  = 1<<pix_desc->log2_chroma_w;
    s->var_values[VAR_VSUB]  = 1<<pix_desc->log2_chroma_h;
    s->var_values[VAR_X]     = NAN;
    s->var_values[VAR_Y]     = NAN;
    s->var_values[VAR_OUT_W] = s->var_values[VAR_OW] = NAN;
    s->var_values[VAR_OUT_H] = s->var_values[VAR_OH] = NAN;
    s->var_values[VAR_N]     = 0;
    s->var_values[VAR_T]     = NAN;
    s->var_values[VAR_POS]   = NAN;
    av_image_fill_max_pixsteps(s->max_step, NULL, pix_desc);
    s->hsub = pix_desc->log2_chroma_w;
    s->vsub = pix_desc->log2_chroma_h;
    if ((ret = av_expr_parse_and_eval(&res, (expr = s->w_expr),
                                      var_names, s->var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
        goto fail_expr;
    s->var_values[VAR_OUT_W] = s->var_values[VAR_OW] = res;
    if ((ret = av_expr_parse_and_eval(&res, (expr = s->h_expr),
                                      var_names, s->var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
        goto fail_expr;
    s->var_values[VAR_OUT_H] = s->var_values[VAR_OH] = res;
    
    if ((ret = av_expr_parse_and_eval(&res, (expr = s->w_expr),
                                      var_names, s->var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
        goto fail_expr;
    s->var_values[VAR_OUT_W] = s->var_values[VAR_OW] = res;
    if (normalize_double(&s->w, s->var_values[VAR_OUT_W]) < 0 ||
        normalize_double(&s->h, s->var_values[VAR_OUT_H]) < 0) {
        av_log(ctx, AV_LOG_ERROR,
               "Too big value or invalid expression for out_w/ow or out_h/oh. "
               "Maybe the expression for out_w:'%s' or for out_h:'%s' is self-referencing.\n",
               s->w_expr, s->h_expr);
        return AVERROR(EINVAL);
    }
    s->w &= ~((1 << s->hsub) - 1);
    s->h &= ~((1 << s->vsub) - 1);
    av_expr_free(s->x_pexpr);
    av_expr_free(s->y_pexpr);
    s->x_pexpr = s->y_pexpr = NULL;
    if ((ret = av_expr_parse(&s->x_pexpr, s->x_expr, var_names,
                             NULL, NULL, NULL, NULL, 0, ctx)) < 0 ||
        (ret = av_expr_parse(&s->y_pexpr, s->y_expr, var_names,
                             NULL, NULL, NULL, NULL, 0, ctx)) < 0)
        return AVERROR(EINVAL);
    if (s->keep_aspect) {
        AVRational dar = av_mul_q(link->sample_aspect_ratio,
                                  (AVRational){ link->w, link->h });
        av_reduce(&s->out_sar.num, &s->out_sar.den,
                  dar.num * s->h, dar.den * s->w, INT_MAX);
    } else
        s->out_sar = link->sample_aspect_ratio;
    av_log(ctx, AV_LOG_VERBOSE, "w:%d h:%d sar:%d/%d -> w:%d h:%d sar:%d/%d\n",
           link->w, link->h, link->sample_aspect_ratio.num, link->sample_aspect_ratio.den,
           s->w, s->h, s->out_sar.num, s->out_sar.den);
    if (s->w <= 0 || s->h <= 0 ||
        s->w > link->w || s->h > link->h) {
        av_log(ctx, AV_LOG_ERROR,
               "Invalid too big or non positive size for width '%d' or height '%d'\n",
               s->w, s->h);
        return AVERROR(EINVAL);
    }
    
    s->x = (link->w - s->w) / 2;
    s->y = (link->h - s->h) / 2;
    s->x &= ~((1 << s->hsub) - 1);
    s->y &= ~((1 << s->vsub) - 1);
    return 0;
fail_expr:
    av_log(NULL, AV_LOG_ERROR, "Error when evaluating the expression '%s'\n", expr);
    return ret;
}
