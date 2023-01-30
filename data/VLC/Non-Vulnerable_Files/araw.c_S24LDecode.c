}
static void S24LDecode( void *outp, const uint8_t *in, unsigned samples )
{
    uint32_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        uint32_t s = ((in[2] << 24) | (in[1] << 16) | (in[0] << 8));
        *(out++) = s;
        in += 3;
    }
}
