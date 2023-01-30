static int sbg_read_seek(AVFormatContext *avf, int stream_index,
                         int64_t ts, int flags)
{
    return sbg_read_seek2(avf, stream_index, ts, ts, ts, 0);
}
