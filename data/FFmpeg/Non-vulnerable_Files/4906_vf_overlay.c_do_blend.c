static AVFrame *do_blend(AVFilterContext *ctx, AVFrame *mainpic,
                         const AVFrame *second)
{
    OverlayContext *s = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    if (s->eval_mode == EVAL_MODE_FRAME) {
        int64_t pos = av_frame_get_pkt_pos(mainpic);
        s->var_values[VAR_N] = inlink->frame_count;
        s->var_values[VAR_T] = mainpic->pts == AV_NOPTS_VALUE ?
            NAN : mainpic->pts * av_q2d(inlink->time_base);
        s->var_values[VAR_POS] = pos == -1 ? NAN : pos;
        eval_expr(ctx);
        av_log(ctx, AV_LOG_DEBUG, "n:%f t:%f pos:%f x:%f xi:%d y:%f yi:%d\n",
               s->var_values[VAR_N], s->var_values[VAR_T], s->var_values[VAR_POS],
               s->var_values[VAR_X], s->x,
               s->var_values[VAR_Y], s->y);
    }
    blend_image(ctx, mainpic, second, s->x, s->y);
    return mainpic;
}
