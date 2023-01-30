/* Convert Unicode code point to UTF-8 */
static char *utf8_cp (uint_fast32_t cp, char *buf)
{
    if (cp < (1 << 7))
    {
        buf[1] = 0;
        buf[0] = cp;
    }
    else if (cp < (1 << 11))
    {
        buf[2] = 0;
        buf[1] = 0x80 | (cp & 0x3F);
        cp >>= 6;
        buf[0] = 0xC0 | cp;
    }
    else if (cp < (1 << 16))
    {
        buf[3] = 0;
        buf[2] = 0x80 | (cp & 0x3F);
        cp >>= 6;
        buf[1] = 0x80 | (cp & 0x3F);
        cp >>= 6;
        buf[0] = 0xE0 | cp;
    }
    else if (cp < (1 << 21))
    {
        buf[4] = 0;
        buf[3] = 0x80 | (cp & 0x3F);
        cp >>= 6;
        buf[2] = 0x80 | (cp & 0x3F);
        cp >>= 6;
        buf[1] = 0x80 | (cp & 0x3F);
        cp >>= 6;
        buf[0] = 0xE0 | cp;
    }
    else
        return NULL;
    return buf;
}
