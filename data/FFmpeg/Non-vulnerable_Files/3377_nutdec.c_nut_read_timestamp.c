static int64_t nut_read_timestamp(AVFormatContext *s, int stream_index,
                                  int64_t *pos_arg, int64_t pos_limit)
{
    NUTContext *nut = s->priv_data;
    AVIOContext *bc = s->pb;
    int64_t pos, pts, back_ptr;
    av_log(s, AV_LOG_DEBUG, "read_timestamp(X,%d,%"PRId64",%"PRId64")\n",
           stream_index, *pos_arg, pos_limit);
    pos = *pos_arg;
    do {
        pos = find_startcode(bc, SYNCPOINT_STARTCODE, pos) + 1;
        if (pos < 1) {
            av_log(s, AV_LOG_ERROR, "read_timestamp failed.\n");
            return AV_NOPTS_VALUE;
        }
    } while (decode_syncpoint(nut, &pts, &back_ptr) < 0);
    *pos_arg = pos - 1;
    av_assert0(nut->last_syncpoint_pos == *pos_arg);
    av_log(s, AV_LOG_DEBUG, "return %"PRId64" %"PRId64"\n", pts, back_ptr);
    if (stream_index == -2)
        return back_ptr;
    av_assert0(stream_index == -1);
    return pts;
}
