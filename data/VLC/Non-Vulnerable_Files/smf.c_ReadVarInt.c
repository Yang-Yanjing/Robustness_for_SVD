 */
static int32_t ReadVarInt (stream_t *s)
{
    uint32_t val = 0;
    uint8_t byte;
    for (unsigned i = 0; i < 4; i++)
    {
        if (stream_Read (s, &byte, 1) < 1)
            return -1;
        val = (val << 7) | (byte & 0x7f);
        if ((byte & 0x80) == 0)
            return val;
    }
    return -1;
}
