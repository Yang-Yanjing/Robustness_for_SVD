}
static void U32IEncode( void *outp, const uint8_t *inp, unsigned samples )
{
    const uint32_t *in = (const uint32_t *)inp;
    uint32_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
        *(out++) =  bswap32( *(in++) + 0x80000000 );
}
