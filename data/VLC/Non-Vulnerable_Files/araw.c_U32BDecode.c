}
static void U32BDecode( void *outp, const uint8_t *in, unsigned samples )
{
    uint32_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        *(out++) = GetDWBE( in ) - 0x80000000;
        in += 4;
    }
}
