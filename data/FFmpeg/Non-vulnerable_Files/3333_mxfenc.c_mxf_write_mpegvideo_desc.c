static void mxf_write_mpegvideo_desc(AVFormatContext *s, AVStream *st)
{
    AVIOContext *pb = s->pb;
    MXFStreamContext *sc = st->priv_data;
    int profile_and_level = (st->codec->profile<<4) | st->codec->level;
    if (st->codec->codec_id != AV_CODEC_ID_H264) {
        mxf_write_cdci_common(s, st, mxf_mpegvideo_descriptor_key, 8+5);
        
        mxf_write_local_tag(pb, 4, 0x8000);
        avio_wb32(pb, sc->video_bit_rate);
        
        mxf_write_local_tag(pb, 1, 0x8007);
        if (!st->codec->profile)
            profile_and_level |= 0x80; 
        avio_w8(pb, profile_and_level);
    } else {
        mxf_write_cdci_common(s, st, mxf_mpegvideo_descriptor_key, 0);
    }
}
