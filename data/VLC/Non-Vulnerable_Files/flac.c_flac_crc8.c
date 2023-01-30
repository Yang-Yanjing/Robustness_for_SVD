};
static uint8_t flac_crc8(const uint8_t *data, unsigned len)
{
    uint8_t crc = 0;
    while (len--)
        crc = flac_crc8_table[crc ^ *data++];
    return crc;
}
