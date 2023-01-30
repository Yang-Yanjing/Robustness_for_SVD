static int flv_read_seek(AVFormatContext *s, int stream_index,
                         int64_t ts, int flags)
{
    FLVContext *flv = s->priv_data;
    flv->validate_count = 0;
    return avio_seek_time(s->pb, stream_index, ts, flags);
}
