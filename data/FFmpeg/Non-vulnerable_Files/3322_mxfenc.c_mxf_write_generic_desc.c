static void mxf_write_generic_desc(AVFormatContext *s, AVStream *st, const UID key, unsigned size)
{
    MXFContext *mxf = s->priv_data;
    MXFStreamContext *sc = st->priv_data;
    AVIOContext *pb = s->pb;
    avio_write(pb, key, 16);
    klv_encode_ber4_length(pb, size+20+8+12+20);
    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, SubDescriptor, st->index);
    mxf_write_local_tag(pb, 4, 0x3006);
    avio_wb32(pb, st->index+2);
    mxf_write_local_tag(pb, 8, 0x3001);
    avio_wb32(pb, mxf->time_base.den);
    avio_wb32(pb, mxf->time_base.num);
    mxf_write_local_tag(pb, 16, 0x3004);
    avio_write(pb, mxf_essence_container_uls[sc->index].container_ul, 16);
}
