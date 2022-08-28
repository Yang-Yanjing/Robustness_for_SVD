static void mxf_write_sequence(AVFormatContext *s, AVStream *st, enum MXFMetadataSetType type)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    enum MXFMetadataSetType component;
    mxf_write_metadata_key(pb, 0x010f00);
    PRINT_KEY(s, "sequence key", pb->buf_ptr - 16);
    klv_encode_ber_length(pb, 80);
    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, type == MaterialPackage ? Sequence: Sequence + TypeBottom, st->index);
    PRINT_KEY(s, "sequence uid", pb->buf_ptr - 16);
    mxf_write_common_fields(s, st);
    
    mxf_write_local_tag(pb, 16 + 8, 0x1001);
    mxf_write_refs_count(pb, 1);
    if (st == mxf->timecode_track)
        component = TimecodeComponent;
    else
        component = SourceClip;
    if (type == SourcePackage)
        component += TypeBottom;
    mxf_write_uuid(pb, component, st->index);
}
