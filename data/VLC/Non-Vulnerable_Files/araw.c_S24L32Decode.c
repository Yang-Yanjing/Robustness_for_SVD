}
static void S24L32Decode( void *outp, const uint8_t *in, unsigned samples )
{
    uint32_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        *(out++) = GetDWLE( in ) << 8;
        in += 4;
    }
}
