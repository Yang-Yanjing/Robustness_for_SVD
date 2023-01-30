}
static void U24BDecode( void *outp, const uint8_t *in, unsigned samples )
{
    uint32_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        uint32_t s = ((in[0] << 24) | (in[1] << 16) | (in[2] << 8)) - 0x80000000;
        *(out++) = s;
        in += 3;
    }
}
