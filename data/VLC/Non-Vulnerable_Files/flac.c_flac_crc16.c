};
static uint16_t flac_crc16(uint16_t crc, uint8_t byte)
{
    return (crc << 8) ^ flac_crc16_table[(crc >> 8) ^ byte];
}
