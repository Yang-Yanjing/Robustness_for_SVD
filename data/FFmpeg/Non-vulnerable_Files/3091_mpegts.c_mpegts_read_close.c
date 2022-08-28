static int mpegts_read_close(AVFormatContext *s)
{
    MpegTSContext *ts = s->priv_data;
    mpegts_free(ts);
    return 0;
}
