static int config_input(AVFilterLink *inlink)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    AVFilterContext    *ctx = inlink->dst;
    BoxBlurContext *s = ctx->priv;
    int w = inlink->w, h = inlink->h;
    int cw, ch;
    double var_values[VARS_NB], res;
    char *expr;
    int ret;
    if (!(s->temp[0] = av_malloc(2*FFMAX(w, h))) ||
        !(s->temp[1] = av_malloc(2*FFMAX(w, h))))
        return AVERROR(ENOMEM);
    s->hsub = desc->log2_chroma_w;
    s->vsub = desc->log2_chroma_h;
    var_values[VAR_W]       = inlink->w;
    var_values[VAR_H]       = inlink->h;
    var_values[VAR_CW] = cw = w>>s->hsub;
    var_values[VAR_CH] = ch = h>>s->vsub;
    var_values[VAR_HSUB]    = 1<<s->hsub;
    var_values[VAR_VSUB]    = 1<<s->vsub;
#define EVAL_RADIUS_EXPR(comp)                                          \
    expr = s->comp##_param.radius_expr;                                 \
    ret = av_expr_parse_and_eval(&res, expr, var_names, var_values,     \
                                 NULL, NULL, NULL, NULL, NULL, 0, ctx); \
    s->comp##_param.radius = res;                                       \
    if (ret < 0) {                                                      \
        av_log(NULL, AV_LOG_ERROR,                                      \
               "Error when evaluating " #comp " radius expression '%s'\n", expr); \
        return ret;                                                     \
    }
    EVAL_RADIUS_EXPR(luma);
    EVAL_RADIUS_EXPR(chroma);
    EVAL_RADIUS_EXPR(alpha);
    av_log(ctx, AV_LOG_VERBOSE,
           "luma_radius:%d luma_power:%d "
           "chroma_radius:%d chroma_power:%d "
           "alpha_radius:%d alpha_power:%d "
           "w:%d chroma_w:%d h:%d chroma_h:%d\n",
           s->luma_param  .radius, s->luma_param  .power,
           s->chroma_param.radius, s->chroma_param.power,
           s->alpha_param .radius, s->alpha_param .power,
           w, cw, h, ch);
#define CHECK_RADIUS_VAL(w_, h_, comp)                                  \
    if (s->comp##_param.radius < 0 ||                                   \
        2*s->comp##_param.radius > FFMIN(w_, h_)) {                     \
        av_log(ctx, AV_LOG_ERROR,                                       \
               "Invalid " #comp " radius value %d, must be >= 0 and <= %d\n", \
               s->comp##_param.radius, FFMIN(w_, h_)/2);                \
        return AVERROR(EINVAL);                                         \
    }
    CHECK_RADIUS_VAL(w,  h,  luma);
    CHECK_RADIUS_VAL(cw, ch, chroma);
    CHECK_RADIUS_VAL(w,  h,  alpha);
    s->radius[Y] = s->luma_param.radius;
    s->radius[U] = s->radius[V] = s->chroma_param.radius;
    s->radius[A] = s->alpha_param.radius;
    s->power[Y] = s->luma_param.power;
    s->power[U] = s->power[V] = s->chroma_param.power;
    s->power[A] = s->alpha_param.power;
    return 0;
}
