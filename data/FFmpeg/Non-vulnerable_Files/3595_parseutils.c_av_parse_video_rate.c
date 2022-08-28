int av_parse_video_rate(AVRational *rate, const char *arg)
{
    int i, ret;
    int n = FF_ARRAY_ELEMS(video_rate_abbrs);
    
    for (i = 0; i < n; ++i)
        if (!strcmp(video_rate_abbrs[i].abbr, arg)) {
            *rate = video_rate_abbrs[i].rate;
            return 0;
        }
    
    if ((ret = av_parse_ratio_quiet(rate, arg, 1001000)) < 0)
        return ret;
    if (rate->num <= 0 || rate->den <= 0)
        return AVERROR(EINVAL);
    return 0;
}
