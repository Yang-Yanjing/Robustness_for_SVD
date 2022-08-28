static int read_seek(AVFormatContext *ctx, int stream_index, int64_t timestamp,
                     int flags)
{
    CDIOContext *s = ctx->priv_data;
    AVStream *st = ctx->streams[0];
    cdio_paranoia_seek(s->paranoia, timestamp, SEEK_SET);
    st->cur_dts = timestamp;
    return 0;
}
