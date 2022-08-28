static void mxf_write_uuid(AVIOContext *pb, enum MXFMetadataSetType type, int value)
{
    avio_write(pb, uuid_base, 12);
    avio_wb16(pb, type);
    avio_wb16(pb, value);
}
