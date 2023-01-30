void ff_printGUID(const GUID *g)
{
#if DSHOWDEBUG
    const uint32_t *d = (const uint32_t *) &g->Data1;
    const uint16_t *w = (const uint16_t *) &g->Data2;
    const uint8_t  *c = (const uint8_t  *) &g->Data4;
    dshowdebug("0x%08x 0x%04x 0x%04x %02x%02x%02x%02x%02x%02x%02x%02x",
               d[0], w[0], w[1],
               c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
#endif
}
