}
static void U32LDecode( void *outp, const uint8_t *in, unsigned samples )
{
    uint32_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        *(out++) = GetDWLE( in ) - 0x80000000;
        in += 4;
    }
}
