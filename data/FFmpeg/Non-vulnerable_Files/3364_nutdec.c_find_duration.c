
static int64_t find_duration(NUTContext *nut, int64_t filesize)
{
    AVFormatContext *s = nut->avf;
    int64_t duration = 0;
    ff_find_last_ts(s, -1, &duration, NULL, nut_read_timestamp);
    if(duration > 0)
        s->duration_estimation_method = AVFMT_DURATION_FROM_PTS;
    return duration;
}
