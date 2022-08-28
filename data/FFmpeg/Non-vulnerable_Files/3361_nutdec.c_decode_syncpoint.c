static int decode_syncpoint(NUTContext *nut, int64_t *ts, int64_t *back_ptr)
{
    AVFormatContext *s = nut->avf;
    AVIOContext *bc    = s->pb;
    int64_t end;
    uint64_t tmp;
    int ret;
    nut->last_syncpoint_pos = avio_tell(bc) - 8;
    end  = get_packetheader(nut, bc, 1, SYNCPOINT_STARTCODE);
    end += avio_tell(bc);
    tmp       = ffio_read_varlen(bc);
    *back_ptr = nut->last_syncpoint_pos - 16 * ffio_read_varlen(bc);
    if (*back_ptr < 0)
        return AVERROR_INVALIDDATA;
    ff_nut_reset_ts(nut, nut->time_base[tmp % nut->time_base_count],
                    tmp / nut->time_base_count);
    if (nut->flags & NUT_BROADCAST) {
        tmp = ffio_read_varlen(bc);
        av_log(s, AV_LOG_VERBOSE, "Syncpoint wallclock %"PRId64"\n",
               av_rescale_q(tmp / nut->time_base_count,
                            nut->time_base[tmp % nut->time_base_count],
                            AV_TIME_BASE_Q));
    }
    if (skip_reserved(bc, end) || ffio_get_checksum(bc)) {
        av_log(s, AV_LOG_ERROR, "sync point checksum mismatch\n");
        return AVERROR_INVALIDDATA;
    }
    *ts = tmp / nut->time_base_count *
          av_q2d(nut->time_base[tmp % nut->time_base_count]) * AV_TIME_BASE;
    if ((ret = ff_nut_add_sp(nut, nut->last_syncpoint_pos, *back_ptr, *ts)) < 0)
        return ret;
    return 0;
}
