static void put_ebml_uint(AVIOContext *pb, unsigned int elementid, uint64_t val)
{
    int i, bytes = 1;
    uint64_t tmp = val;
    while (tmp >>= 8)
        bytes++;
    put_ebml_id(pb, elementid);
    put_ebml_num(pb, bytes, 0);
    for (i = bytes - 1; i >= 0; i--)
        avio_w8(pb, (uint8_t)(val >> i * 8));
}
