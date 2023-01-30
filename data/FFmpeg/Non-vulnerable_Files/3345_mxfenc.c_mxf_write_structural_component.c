static void mxf_write_structural_component(AVFormatContext *s, AVStream *st, enum MXFMetadataSetType type)
{
    AVIOContext *pb = s->pb;
    int i;
    mxf_write_metadata_key(pb, 0x011100);
    PRINT_KEY(s, "sturctural component key", pb->buf_ptr - 16);
    klv_encode_ber_length(pb, 108);
    
    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, type == MaterialPackage ? SourceClip: SourceClip + TypeBottom, st->index);
    PRINT_KEY(s, "structural component uid", pb->buf_ptr - 16);
    mxf_write_common_fields(s, st);
    
    mxf_write_local_tag(pb, 8, 0x1201);
    avio_wb64(pb, 0);
    
    mxf_write_local_tag(pb, 32, 0x1101);
    if (type == SourcePackage) {
        for (i = 0; i < 4; i++)
            avio_wb64(pb, 0);
    } else
        mxf_write_umid(s, 1);
    
    mxf_write_local_tag(pb, 4, 0x1102);
    if (type == SourcePackage)
        avio_wb32(pb, 0);
    else
        avio_wb32(pb, st->index+2);
}
