static int get_packetheader(NUTContext *nut, AVIOContext *bc,
                            int calculate_checksum, uint64_t startcode)
{
    int64_t size;

    startcode = av_be2ne64(startcode);
    startcode = ff_crc04C11DB7_update(0, (uint8_t*) &startcode, 8);
    ffio_init_checksum(bc, ff_crc04C11DB7_update, startcode);
    size = ffio_read_varlen(bc);
    if (size > 4096)
        avio_rb32(bc);
    if (ffio_get_checksum(bc) && size > 4096)
        return -1;
    ffio_init_checksum(bc, calculate_checksum ? ff_crc04C11DB7_update : NULL, 0);
    return size;
}
