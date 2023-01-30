static int filter_frame(AVFilterLink *inlink, AVFrame *inpicref)
{
    OverlayContext *s = inlink->dst->priv;
    av_log(inlink->dst, AV_LOG_DEBUG, "Incoming frame (time:%s) from link #%d\n", av_ts2timestr(inpicref->pts, &inlink->time_base), FF_INLINK_IDX(inlink));
    return ff_dualinput_filter_frame(&s->dinput, inlink, inpicref);
}
