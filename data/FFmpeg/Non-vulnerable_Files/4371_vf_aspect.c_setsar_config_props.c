static int setsar_config_props(AVFilterLink *inlink)
{
    AspectContext *s = inlink->dst->priv;
    AVRational old_sar = inlink->sample_aspect_ratio;
    AVRational old_dar, dar;
    int ret;
#if FF_API_OLD_FILTER_OPTS
    if (!(s->ratio_expr && s->aspect_den > 0)) {
#endif
    if ((ret = get_aspect_ratio(inlink, &s->sar)))
        return ret;
#if FF_API_OLD_FILTER_OPTS
    }
#endif
    inlink->sample_aspect_ratio = s->sar;
    compute_dar(&old_dar, old_sar, inlink->w, inlink->h);
    compute_dar(&dar, s->sar, inlink->w, inlink->h);
    av_log(inlink->dst, AV_LOG_VERBOSE, "w:%d h:%d sar:%d/%d dar:%d/%d -> sar:%d/%d dar:%d/%d\n",
           inlink->w, inlink->h, old_sar.num, old_sar.den, old_dar.num, old_dar.den,
           inlink->sample_aspect_ratio.num, inlink->sample_aspect_ratio.den, dar.num, dar.den);
    return 0;
}
