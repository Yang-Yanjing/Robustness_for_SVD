static void mxf_write_klv_fill(AVFormatContext *s)
{
    unsigned pad = klv_fill_size(avio_tell(s->pb));
    if (pad) {
        avio_write(s->pb, klv_fill_key, 16);
        pad -= 16 + 4;
        klv_encode_ber4_length(s->pb, pad);
        ffio_fill(s->pb, 0, pad);
        av_assert1(!(avio_tell(s->pb) & (KAG_SIZE-1)));
    }
}
