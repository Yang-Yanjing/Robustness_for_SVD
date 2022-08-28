static int setdar_config_props(AVFilterLink *inlink)
{
    AspectContext *s = inlink->dst->priv;
    AVRational dar;
    AVRational old_dar;
    AVRational old_sar = inlink->sample_aspect_ratio;
    int ret;
#if FF_API_OLD_FILTER_OPTS
    if (!(s->ratio_expr && s->aspect_den > 0)) {
#endif
    if ((ret = get_aspect_ratio(inlink, &s->dar)))
        return ret;
#if FF_API_OLD_FILTER_OPTS
    }
#endif
    if (s->dar.num && s->dar.den) {
        av_reduce(&s->sar.num, &s->sar.den,
                   s->dar.num * inlink->h,
                   s->dar.den * inlink->w, INT_MAX);
        inlink->sample_aspect_ratio = s->sar;
        dar = s->dar;
    } else {
        inlink->sample_aspect_ratio = (AVRational){ 1, 1 };
        dar = (AVRational){ inlink->w, inlink->h };
    }
    compute_dar(&old_dar, old_sar, inlink->w, inlink->h);
    av_log(inlink->dst, AV_LOG_VERBOSE, "w:%d h:%d dar:%d/%d sar:%d/%d -> dar:%d/%d sar:%d/%d\n",
           inlink->w, inlink->h, old_dar.num, old_dar.den, old_sar.num, old_sar.den,
           dar.num, dar.den, inlink->sample_aspect_ratio.num, inlink->sample_aspect_ratio.den);
    return 0;
}
