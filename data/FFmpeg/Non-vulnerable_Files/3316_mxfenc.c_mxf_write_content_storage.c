static void mxf_write_content_storage(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    mxf_write_metadata_key(pb, 0x011800);
    PRINT_KEY(s, "content storage key", pb->buf_ptr - 16);
    klv_encode_ber_length(pb, 92);
    
    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, ContentStorage, 0);
    PRINT_KEY(s, "content storage uid", pb->buf_ptr - 16);
    
    mxf_write_local_tag(pb, 16 * 2 + 8, 0x1901);
    mxf_write_refs_count(pb, 2);
    mxf_write_uuid(pb, MaterialPackage, 0);
    mxf_write_uuid(pb, SourcePackage, 0);
    
    mxf_write_local_tag(pb, 8 + 16, 0x1902);
    mxf_write_refs_count(pb, 1);
    mxf_write_uuid(pb, EssenceContainerData, 0);
}
