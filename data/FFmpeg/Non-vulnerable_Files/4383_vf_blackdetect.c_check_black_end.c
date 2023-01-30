static void check_black_end(AVFilterContext *ctx)
{
    BlackDetectContext *blackdetect = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    if ((blackdetect->black_end - blackdetect->black_start) >= blackdetect->black_min_duration) {
        av_log(blackdetect, AV_LOG_INFO,
               "black_start:%s black_end:%s black_duration:%s\n",
               av_ts2timestr(blackdetect->black_start, &inlink->time_base),
               av_ts2timestr(blackdetect->black_end,   &inlink->time_base),
               av_ts2timestr(blackdetect->black_end - blackdetect->black_start, &inlink->time_base));
    }
}
