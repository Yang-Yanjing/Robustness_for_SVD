static void klv_encode_ber4_length(AVIOContext *pb, int len)
{
    avio_w8(pb, 0x80 + 3);
    avio_wb24(pb, len);
}
