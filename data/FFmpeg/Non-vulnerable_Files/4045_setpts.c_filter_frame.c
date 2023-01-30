static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    SetPTSContext *setpts = inlink->dst->priv;
    int64_t in_pts = frame->pts;
    double d;
    if (isnan(setpts->var_values[VAR_STARTPTS])) {
        setpts->var_values[VAR_STARTPTS] = TS2D(frame->pts);
        setpts->var_values[VAR_STARTT  ] = TS2T(frame->pts, inlink->time_base);
    }
    setpts->var_values[VAR_PTS       ] = TS2D(frame->pts);
    setpts->var_values[VAR_T         ] = TS2T(frame->pts, inlink->time_base);
    setpts->var_values[VAR_POS       ] = av_frame_get_pkt_pos(frame) == -1 ? NAN : av_frame_get_pkt_pos(frame);
    setpts->var_values[VAR_RTCTIME   ] = av_gettime();
    if (inlink->type == AVMEDIA_TYPE_VIDEO) {
        setpts->var_values[VAR_INTERLACED] = frame->interlaced_frame;
    } else if (inlink->type == AVMEDIA_TYPE_AUDIO) {
        setpts->var_values[VAR_S] = frame->nb_samples;
        setpts->var_values[VAR_NB_SAMPLES] = frame->nb_samples;
    }
    d = av_expr_eval(setpts->expr, setpts->var_values, NULL);
    frame->pts = D2TS(d);
    av_log(inlink->dst, AV_LOG_TRACE,
            "N:%"PRId64" PTS:%s T:%f POS:%s",
            (int64_t)setpts->var_values[VAR_N],
            d2istr(setpts->var_values[VAR_PTS]),
            setpts->var_values[VAR_T],
            d2istr(setpts->var_values[VAR_POS]));
    switch (inlink->type) {
    case AVMEDIA_TYPE_VIDEO:
        av_log(inlink->dst, AV_LOG_TRACE, " INTERLACED:%"PRId64,
                (int64_t)setpts->var_values[VAR_INTERLACED]);
        break;
    case AVMEDIA_TYPE_AUDIO:
        av_log(inlink->dst, AV_LOG_TRACE, " NB_SAMPLES:%"PRId64" NB_CONSUMED_SAMPLES:%"PRId64,
                (int64_t)setpts->var_values[VAR_NB_SAMPLES],
                (int64_t)setpts->var_values[VAR_NB_CONSUMED_SAMPLES]);
        break;
    }
    av_log(inlink->dst, AV_LOG_TRACE, " -> PTS:%s T:%f\n", d2istr(d), TS2T(d, inlink->time_base));
    if (inlink->type == AVMEDIA_TYPE_VIDEO) {
        setpts->var_values[VAR_N] += 1.0;
    } else {
        setpts->var_values[VAR_N] += frame->nb_samples;
    }
    setpts->var_values[VAR_PREV_INPTS ] = TS2D(in_pts);
    setpts->var_values[VAR_PREV_INT   ] = TS2T(in_pts, inlink->time_base);
    setpts->var_values[VAR_PREV_OUTPTS] = TS2D(frame->pts);
    setpts->var_values[VAR_PREV_OUTT]   = TS2T(frame->pts, inlink->time_base);
    if (setpts->type == AVMEDIA_TYPE_AUDIO) {
        setpts->var_values[VAR_NB_CONSUMED_SAMPLES] += frame->nb_samples;
    }
    return ff_filter_frame(inlink->dst->outputs[0], frame);
}
