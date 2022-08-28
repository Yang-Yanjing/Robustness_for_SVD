static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    BlackDetectContext *blackdetect = ctx->priv;
    blackdetect->black_min_duration =
        blackdetect->black_min_duration_time / av_q2d(inlink->time_base);
    blackdetect->pixel_black_th_i = ff_fmt_is_in(inlink->format, yuvj_formats) ?
        
             blackdetect->pixel_black_th *  255 :
        16 + blackdetect->pixel_black_th * (235 - 16);
    av_log(blackdetect, AV_LOG_VERBOSE,
           "black_min_duration:%s pixel_black_th:%f pixel_black_th_i:%d picture_black_ratio_th:%f\n",
           av_ts2timestr(blackdetect->black_min_duration, &inlink->time_base),
           blackdetect->pixel_black_th, blackdetect->pixel_black_th_i,
           blackdetect->picture_black_ratio_th);
    return 0;
}
