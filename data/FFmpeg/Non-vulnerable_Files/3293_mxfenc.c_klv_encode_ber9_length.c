static void klv_encode_ber9_length(AVIOContext *pb, uint64_t len)
{
    avio_w8(pb, 0x80 + 8);
    avio_wb64(pb, len);
}
