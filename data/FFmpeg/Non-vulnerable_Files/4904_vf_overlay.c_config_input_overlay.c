static int config_input_overlay(AVFilterLink *inlink)
{
    AVFilterContext *ctx  = inlink->dst;
    OverlayContext  *s = inlink->dst->priv;
    int ret;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(inlink->format);
    av_image_fill_max_pixsteps(s->overlay_pix_step, NULL, pix_desc);
    

    s->var_values[VAR_MAIN_W   ] = s->var_values[VAR_MW] = ctx->inputs[MAIN   ]->w;
    s->var_values[VAR_MAIN_H   ] = s->var_values[VAR_MH] = ctx->inputs[MAIN   ]->h;
    s->var_values[VAR_OVERLAY_W] = s->var_values[VAR_OW] = ctx->inputs[OVERLAY]->w;
    s->var_values[VAR_OVERLAY_H] = s->var_values[VAR_OH] = ctx->inputs[OVERLAY]->h;
    s->var_values[VAR_HSUB]  = 1<<pix_desc->log2_chroma_w;
    s->var_values[VAR_VSUB]  = 1<<pix_desc->log2_chroma_h;
    s->var_values[VAR_X]     = NAN;
    s->var_values[VAR_Y]     = NAN;
    s->var_values[VAR_N]     = 0;
    s->var_values[VAR_T]     = NAN;
    s->var_values[VAR_POS]   = NAN;
    if ((ret = set_expr(&s->x_pexpr,      s->x_expr,      "x",      ctx)) < 0 ||
        (ret = set_expr(&s->y_pexpr,      s->y_expr,      "y",      ctx)) < 0)
        return ret;
    s->overlay_is_packed_rgb =
        ff_fill_rgba_map(s->overlay_rgba_map, inlink->format) >= 0;
    s->overlay_has_alpha = ff_fmt_is_in(inlink->format, alpha_pix_fmts);
    if (s->eval_mode == EVAL_MODE_INIT) {
        eval_expr(ctx);
        av_log(ctx, AV_LOG_VERBOSE, "x:%f xi:%d y:%f yi:%d\n",
               s->var_values[VAR_X], s->x,
               s->var_values[VAR_Y], s->y);
    }
    av_log(ctx, AV_LOG_VERBOSE,
           "main w:%d h:%d fmt:%s overlay w:%d h:%d fmt:%s eof_action:%s\n",
           ctx->inputs[MAIN]->w, ctx->inputs[MAIN]->h,
           av_get_pix_fmt_name(ctx->inputs[MAIN]->format),
           ctx->inputs[OVERLAY]->w, ctx->inputs[OVERLAY]->h,
           av_get_pix_fmt_name(ctx->inputs[OVERLAY]->format),
           eof_action_str[s->eof_action]);
    return 0;
}
