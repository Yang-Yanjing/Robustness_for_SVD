static void mxf_write_metadata_key(AVIOContext *pb, unsigned int value)
{
    avio_write(pb, header_metadata_key, 13);
    avio_wb24(pb, value);
}
