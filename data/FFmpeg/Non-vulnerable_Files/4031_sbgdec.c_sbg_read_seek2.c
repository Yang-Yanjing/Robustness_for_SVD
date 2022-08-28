static int sbg_read_seek2(AVFormatContext *avf, int stream_index,
                          int64_t min_ts, int64_t ts, int64_t max_ts, int flags)
{
    if (flags || stream_index > 0)
        return AVERROR(EINVAL);
    if (stream_index < 0)
        ts = av_rescale_q(ts, AV_TIME_BASE_Q, avf->streams[0]->time_base);
    avf->streams[0]->cur_dts = ts;
    return 0;
}
