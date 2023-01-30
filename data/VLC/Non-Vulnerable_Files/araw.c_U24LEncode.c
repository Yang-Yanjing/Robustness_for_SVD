}
static void U24LEncode( void *outp, const uint8_t *inp, unsigned samples )
{
    const uint32_t *in = (const uint32_t *)inp;
    uint8_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        uint32_t s = *(in++);
        *(out++) = (s >>  8);
        *(out++) = (s >> 16);
        *(out++) = (s >> 24) + 0x80;
    }
}
