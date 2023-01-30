static int filter_frame(AVFilterLink *link, AVFrame *frame)
{
    AVFilterContext *ctx = link->dst;
    CropContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(link->format);
    int i;
    frame->width  = s->w;
    frame->height = s->h;
    s->var_values[VAR_N] = link->frame_count;
    s->var_values[VAR_T] = frame->pts == AV_NOPTS_VALUE ?
        NAN : frame->pts * av_q2d(link->time_base);
    s->var_values[VAR_POS] = av_frame_get_pkt_pos(frame) == -1 ?
        NAN : av_frame_get_pkt_pos(frame);
    s->var_values[VAR_X] = av_expr_eval(s->x_pexpr, s->var_values, NULL);
    s->var_values[VAR_Y] = av_expr_eval(s->y_pexpr, s->var_values, NULL);
    s->var_values[VAR_X] = av_expr_eval(s->x_pexpr, s->var_values, NULL);
    normalize_double(&s->x, s->var_values[VAR_X]);
    normalize_double(&s->y, s->var_values[VAR_Y]);
    if (s->x < 0)
        s->x = 0;
    if (s->y < 0)
        s->y = 0;
    if ((unsigned)s->x + (unsigned)s->w > link->w)
        s->x = link->w - s->w;
    if ((unsigned)s->y + (unsigned)s->h > link->h)
        s->y = link->h - s->h;
    s->x &= ~((1 << s->hsub) - 1);
    s->y &= ~((1 << s->vsub) - 1);
    av_log(ctx, AV_LOG_TRACE, "n:%d t:%f pos:%f x:%d y:%d x+w:%d y+h:%d\n",
            (int)s->var_values[VAR_N], s->var_values[VAR_T], s->var_values[VAR_POS],
            s->x, s->y, s->x+s->w, s->y+s->h);
    frame->data[0] += s->y * frame->linesize[0];
    frame->data[0] += s->x * s->max_step[0];
    if (!(desc->flags & AV_PIX_FMT_FLAG_PAL || desc->flags & AV_PIX_FMT_FLAG_PSEUDOPAL)) {
        for (i = 1; i < 3; i ++) {
            if (frame->data[i]) {
                frame->data[i] += (s->y >> s->vsub) * frame->linesize[i];
                frame->data[i] += (s->x * s->max_step[i]) >> s->hsub;
            }
        }
    }
    
    if (frame->data[3]) {
        frame->data[3] += s->y * frame->linesize[3];
        frame->data[3] += s->x * s->max_step[3];
    }
    return ff_filter_frame(link->dst->outputs[0], frame);
}
