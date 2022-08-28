static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    DrawTextContext *s = ctx->priv;
    int ret;
    if (s->reload) {
        if ((ret = load_textfile(ctx)) < 0) {
            av_frame_free(&frame);
            return ret;
        }
#if CONFIG_LIBFRIBIDI
        if (s->text_shaping)
            if ((ret = shape_text(ctx)) < 0) {
                av_frame_free(&frame);
                return ret;
            }
#endif
    }
    s->var_values[VAR_N] = inlink->frame_count+s->start_number;
    s->var_values[VAR_T] = frame->pts == AV_NOPTS_VALUE ?
        NAN : frame->pts * av_q2d(inlink->time_base);
    s->var_values[VAR_PICT_TYPE] = frame->pict_type;
    s->metadata = av_frame_get_metadata(frame);
    draw_text(ctx, frame, frame->width, frame->height);
    av_log(ctx, AV_LOG_DEBUG, "n:%d t:%f text_w:%d text_h:%d x:%d y:%d\n",
           (int)s->var_values[VAR_N], s->var_values[VAR_T],
           (int)s->var_values[VAR_TEXT_W], (int)s->var_values[VAR_TEXT_H],
           s->x, s->y);
    return ff_filter_frame(outlink, frame);
}
