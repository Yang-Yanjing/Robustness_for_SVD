static void update_context(VignetteContext *s, AVFilterLink *inlink, AVFrame *frame)
{
    int x, y;
    float *dst = s->fmap;
    int dst_linesize = s->fmap_linesize;
    if (frame) {
        s->var_values[VAR_N]   = inlink->frame_count;
        s->var_values[VAR_T]   = TS2T(frame->pts, inlink->time_base);
        s->var_values[VAR_PTS] = TS2D(frame->pts);
    } else {
        s->var_values[VAR_N]   = NAN;
        s->var_values[VAR_T]   = NAN;
        s->var_values[VAR_PTS] = NAN;
    }
    s->angle = av_expr_eval(s->angle_pexpr, s->var_values, NULL);
    s->x0 = av_expr_eval(s->x0_pexpr, s->var_values, NULL);
    s->y0 = av_expr_eval(s->y0_pexpr, s->var_values, NULL);
    if (isnan(s->x0) || isnan(s->y0) || isnan(s->angle))
        s->eval_mode = EVAL_MODE_FRAME;
    s->angle = av_clipf(s->angle, 0, M_PI_2);
    if (s->backward) {
        for (y = 0; y < inlink->h; y++) {
            for (x = 0; x < inlink->w; x++)
                dst[x] = 1. / get_natural_factor(s, x, y);
            dst += dst_linesize;
        }
    } else {
        for (y = 0; y < inlink->h; y++) {
            for (x = 0; x < inlink->w; x++)
                dst[x] = get_natural_factor(s, x, y);
            dst += dst_linesize;
        }
    }
}
