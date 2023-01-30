static int mpegts_resync(AVFormatContext *s)
{
    MpegTSContext *ts = s->priv_data;
    AVIOContext *pb = s->pb;
    int c, i;
    for (i = 0; i < ts->resync_size; i++) {
        c = avio_r8(pb);
        if (avio_feof(pb))
            return AVERROR_EOF;
        if (c == 0x47) {
            avio_seek(pb, -1, SEEK_CUR);
            reanalyze(s->priv_data);
            return 0;
        }
    }
    av_log(s, AV_LOG_ERROR,
           "max resync size reached, could not find sync byte\n");
    
    return AVERROR_INVALIDDATA;
}
