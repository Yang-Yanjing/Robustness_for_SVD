}
static void U16LDecode( void *outp, const uint8_t *in, unsigned samples )
{
    uint16_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        *(out++) = GetWLE( in ) - 0x8000;
        in += 2;
    }
}
