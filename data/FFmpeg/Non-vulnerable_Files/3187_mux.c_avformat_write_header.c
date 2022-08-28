int avformat_write_header(AVFormatContext *s, AVDictionary **options)
{
    int ret = 0;
    if ((ret = init_muxer(s, options)) < 0)
        return ret;
    if (s->oformat->write_header) {
        ret = s->oformat->write_header(s);
        if (ret >= 0 && s->pb && s->pb->error < 0)
            ret = s->pb->error;
        if (ret < 0)
            return ret;
        if (s->flush_packets && s->pb && s->pb->error >= 0 && s->flags & AVFMT_FLAG_FLUSH_PACKETS)
            avio_flush(s->pb);
    }
    if ((ret = init_pts(s)) < 0)
        return ret;
    if (s->avoid_negative_ts < 0) {
        av_assert2(s->avoid_negative_ts == AVFMT_AVOID_NEG_TS_AUTO);
        if (s->oformat->flags & (AVFMT_TS_NEGATIVE | AVFMT_NOTIMESTAMPS)) {
            s->avoid_negative_ts = 0;
        } else
            s->avoid_negative_ts = AVFMT_AVOID_NEG_TS_MAKE_NON_NEGATIVE;
    }
    return 0;
}
