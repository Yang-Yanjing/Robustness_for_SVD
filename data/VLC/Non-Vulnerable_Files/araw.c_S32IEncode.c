}
static void S32IEncode( void *outp, const uint8_t *inp, unsigned samples )
{
    const int32_t *in = (const int32_t *)inp;
    int32_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
        *(out++) = bswap32( *(in++) );
}
