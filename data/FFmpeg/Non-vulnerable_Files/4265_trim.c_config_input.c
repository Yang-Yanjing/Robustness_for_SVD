static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    TrimContext       *s = ctx->priv;
    AVRational tb = (inlink->type == AVMEDIA_TYPE_VIDEO) ?
                     inlink->time_base : (AVRational){ 1, inlink->sample_rate };
    if (s->start_time_dbl != DBL_MAX)
        s->start_time = s->start_time_dbl * 1e6;
    if (s->end_time_dbl != DBL_MAX)
        s->end_time = s->end_time_dbl * 1e6;
    if (s->duration_dbl != 0)
        s->duration = s->duration_dbl * 1e6;
    if (s->start_time != INT64_MAX) {
        int64_t start_pts = av_rescale_q(s->start_time, AV_TIME_BASE_Q, tb);
        if (s->start_pts == AV_NOPTS_VALUE || start_pts < s->start_pts)
            s->start_pts = start_pts;
    }
    if (s->end_time != INT64_MAX) {
        int64_t end_pts = av_rescale_q(s->end_time, AV_TIME_BASE_Q, tb);
        if (s->end_pts == AV_NOPTS_VALUE || end_pts > s->end_pts)
            s->end_pts = end_pts;
    }
    if (s->duration)
        s->duration_tb = av_rescale_q(s->duration, AV_TIME_BASE_Q, tb);
    return 0;
}
