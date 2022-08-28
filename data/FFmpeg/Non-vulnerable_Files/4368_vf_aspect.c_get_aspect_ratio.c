static int get_aspect_ratio(AVFilterLink *inlink, AVRational *aspect_ratio)
{
    AVFilterContext *ctx = inlink->dst;
    AspectContext *s = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    double var_values[VARS_NB], res;
    int ret;
    var_values[VAR_W]     = inlink->w;
    var_values[VAR_H]     = inlink->h;
    var_values[VAR_A]     = (double) inlink->w / inlink->h;
    var_values[VAR_SAR]   = inlink->sample_aspect_ratio.num ?
        (double) inlink->sample_aspect_ratio.num / inlink->sample_aspect_ratio.den : 1;
    var_values[VAR_DAR]   = var_values[VAR_A] * var_values[VAR_SAR];
    var_values[VAR_HSUB]  = 1 << desc->log2_chroma_w;
    var_values[VAR_VSUB]  = 1 << desc->log2_chroma_h;
    
    ret = av_expr_parse_and_eval(&res, s->ratio_expr,
                                      var_names, var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, ctx);
    if (ret < 0) {
        ret = av_parse_ratio(aspect_ratio, s->ratio_expr, s->max, 0, ctx);
    } else
        *aspect_ratio = av_d2q(res, s->max);
    if (ret < 0) {
        av_log(ctx, AV_LOG_ERROR,
               "Error when evaluating the expression '%s'\n", s->ratio_expr);
        return ret;
    }
    if (aspect_ratio->num < 0 || aspect_ratio->den <= 0) {
        av_log(ctx, AV_LOG_ERROR,
                "Invalid string '%s' for aspect ratio\n", s->ratio_expr);
        return AVERROR(EINVAL);
    }
    return 0;
}
