static int mxf_write_opatom_body_partition(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    AVStream *st = s->streams[0];
    MXFStreamContext *sc = st->priv_data;
    const uint8_t *key = NULL;
    int err;
    if (!mxf->header_written)
        key = body_partition_key;
    if ((err = mxf_write_partition(s, 1, 0, key, 0)) < 0)
        return err;
    mxf_write_klv_fill(s);
    avio_write(pb, sc->track_essence_element_key, 16);
    klv_encode_ber9_length(pb, mxf->body_offset);
    return 0;
}
