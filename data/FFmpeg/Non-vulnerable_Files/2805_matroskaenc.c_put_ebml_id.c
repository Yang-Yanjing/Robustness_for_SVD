static void put_ebml_id(AVIOContext *pb, unsigned int id)
{
    int i = ebml_id_size(id);
    while (i--)
        avio_w8(pb, (uint8_t)(id >> (i * 8)));
}
