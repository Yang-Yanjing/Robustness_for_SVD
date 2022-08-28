static int mxf_write_essence_container_data(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    mxf_write_metadata_key(pb, 0x012300);
    klv_encode_ber_length(pb, 72);
    mxf_write_local_tag(pb, 16, 0x3C0A); 
    mxf_write_uuid(pb, EssenceContainerData, 0);
    mxf_write_local_tag(pb, 32, 0x2701); 
    mxf_write_umid(s, 1);
    mxf_write_local_tag(pb, 4, 0x3F07); 
    avio_wb32(pb, 1);
    mxf_write_local_tag(pb, 4, 0x3F06); 
    avio_wb32(pb, 2);
    return 0;
}
