static int parse_pcr(int64_t *ppcr_high, int *ppcr_low, const uint8_t *packet)
{
    int afc, len, flags;
    const uint8_t *p;
    unsigned int v;
    afc = (packet[3] >> 4) & 3;
    if (afc <= 1)
        return AVERROR_INVALIDDATA;
    p   = packet + 4;
    len = p[0];
    p++;
    if (len == 0)
        return AVERROR_INVALIDDATA;
    flags = *p++;
    len--;
    if (!(flags & 0x10))
        return AVERROR_INVALIDDATA;
    if (len < 6)
        return AVERROR_INVALIDDATA;
    v          = AV_RB32(p);
    *ppcr_high = ((int64_t) v << 1) | (p[4] >> 7);
    *ppcr_low  = ((p[4] & 1) << 8) | p[5];
    return 0;
}
