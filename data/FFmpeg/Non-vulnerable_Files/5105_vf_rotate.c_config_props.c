static int config_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    RotContext *rot = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    const AVPixFmtDescriptor *pixdesc = av_pix_fmt_desc_get(inlink->format);
    int ret;
    double res;
    char *expr;
    ff_draw_init(&rot->draw, inlink->format, 0);
    ff_draw_color(&rot->draw, &rot->color, rot->fillcolor);
    rot->hsub = pixdesc->log2_chroma_w;
    rot->vsub = pixdesc->log2_chroma_h;
    rot->var_values[VAR_IN_W] = rot->var_values[VAR_IW] = inlink->w;
    rot->var_values[VAR_IN_H] = rot->var_values[VAR_IH] = inlink->h;
    rot->var_values[VAR_HSUB] = 1<<rot->hsub;
    rot->var_values[VAR_VSUB] = 1<<rot->vsub;
    rot->var_values[VAR_N] = NAN;
    rot->var_values[VAR_T] = NAN;
    rot->var_values[VAR_OUT_W] = rot->var_values[VAR_OW] = NAN;
    rot->var_values[VAR_OUT_H] = rot->var_values[VAR_OH] = NAN;
    av_expr_free(rot->angle_expr);
    rot->angle_expr = NULL;
    if ((ret = av_expr_parse(&rot->angle_expr, expr = rot->angle_expr_str, var_names,
                             func1_names, func1, NULL, NULL, 0, ctx)) < 0) {
        av_log(ctx, AV_LOG_ERROR,
               "Error occurred parsing angle expression '%s'\n", rot->angle_expr_str);
        return ret;
    }
#define SET_SIZE_EXPR(name, opt_name) do {                                         \
    ret = av_expr_parse_and_eval(&res, expr = rot->name##_expr_str,                \
                                 var_names, rot->var_values,                       \
                                 func1_names, func1, NULL, NULL, rot, 0, ctx);     \
    if (ret < 0 || isnan(res) || isinf(res) || res <= 0) {                         \
        av_log(ctx, AV_LOG_ERROR,                                                  \
               "Error parsing or evaluating expression for option %s: "            \
               "invalid expression '%s' or non-positive or indefinite value %f\n", \
               opt_name, expr, res);                                               \
        return ret;                                                                \
    }                                                                              \
} while (0)
    
    av_expr_parse_and_eval(&res, expr = rot->outw_expr_str, var_names, rot->var_values,
                           func1_names, func1, NULL, NULL, rot, 0, ctx);
    rot->var_values[VAR_OUT_W] = rot->var_values[VAR_OW] = res;
    rot->outw = res + 0.5;
    SET_SIZE_EXPR(outh, "out_w");
    rot->var_values[VAR_OUT_H] = rot->var_values[VAR_OH] = res;
    rot->outh = res + 0.5;
    
    SET_SIZE_EXPR(outw, "out_h");
    rot->var_values[VAR_OUT_W] = rot->var_values[VAR_OW] = res;
    rot->outw = res + 0.5;
    
    rot->nb_planes = av_pix_fmt_count_planes(inlink->format);
    outlink->w = rot->outw;
    outlink->h = rot->outh;
    return 0;
}
