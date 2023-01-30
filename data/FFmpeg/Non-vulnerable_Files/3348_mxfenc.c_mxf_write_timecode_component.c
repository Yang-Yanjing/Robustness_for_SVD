static void mxf_write_timecode_component(AVFormatContext *s, AVStream *st, enum MXFMetadataSetType type)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    mxf_write_metadata_key(pb, 0x011400);
    klv_encode_ber_length(pb, 75);
    
    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, type == MaterialPackage ? TimecodeComponent :
                   TimecodeComponent + TypeBottom, st->index);
    mxf_write_common_fields(s, st);
    
    mxf_write_local_tag(pb, 8, 0x1501);
    avio_wb64(pb, mxf->tc.start);
    
    mxf_write_local_tag(pb, 2, 0x1502);
    avio_wb16(pb, mxf->timecode_base);
    
    mxf_write_local_tag(pb, 1, 0x1503);
    avio_w8(pb, !!(mxf->tc.flags & AV_TIMECODE_FLAG_DROPFRAME));
}
