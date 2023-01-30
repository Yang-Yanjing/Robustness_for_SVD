static void mxf_write_local_tag(AVIOContext *pb, int size, int tag)
{
    avio_wb16(pb, tag);
    avio_wb16(pb, size);
}
