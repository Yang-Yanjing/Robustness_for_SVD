static unsigned crc32(const uint8_t *data, unsigned size)
{
    return av_crc(av_crc_get_table(AV_CRC_32_IEEE), 0, data, size);
}
