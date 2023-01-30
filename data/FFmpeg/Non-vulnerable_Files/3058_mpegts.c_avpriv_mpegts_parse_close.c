void avpriv_mpegts_parse_close(MpegTSContext *ts)
{
    mpegts_free(ts);
    av_free(ts);
}
