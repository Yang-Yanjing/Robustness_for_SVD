int av_timecode_init_from_string(AVTimecode *tc, AVRational rate, const char *str, void *log_ctx)
{
    char c;
    int hh, mm, ss, ff, ret;
    if (sscanf(str, "%d:%d:%d%c%d", &hh, &mm, &ss, &c, &ff) != 5) {
        av_log(log_ctx, AV_LOG_ERROR, "Unable to parse timecode, "
                                      "syntax: hh:mm:ss[:;.]ff\n");
        return AVERROR_INVALIDDATA;
    }
    memset(tc, 0, sizeof(*tc));
    tc->flags = c != ':' ? AV_TIMECODE_FLAG_DROPFRAME : 0; 
    tc->rate  = rate;
    tc->fps   = fps_from_frame_rate(rate);
    ret = check_timecode(log_ctx, tc);
    if (ret < 0)
        return ret;
    tc->start = (hh*3600 + mm*60 + ss) * tc->fps + ff;
    if (tc->flags & AV_TIMECODE_FLAG_DROPFRAME) { 
        int tmins = 60*hh + mm;
        tc->start -= 2 * (tmins - tmins/10);
    }
    return 0;
}
