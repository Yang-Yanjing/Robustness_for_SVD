static void mxf_write_wav_common(AVFormatContext *s, AVStream *st, const UID key, unsigned size)
{
    AVIOContext *pb = s->pb;
    mxf_write_generic_sound_common(s, st, key, size+6+8);
    mxf_write_local_tag(pb, 2, 0x3D0A);
    avio_wb16(pb, st->codec->block_align);
    
    mxf_write_local_tag(pb, 4, 0x3D09);
    avio_wb32(pb, st->codec->block_align*st->codec->sample_rate);
}
