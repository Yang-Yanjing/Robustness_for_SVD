}
static void U16NEncode( void *outp, const uint8_t *inp, unsigned samples )
{
    const uint16_t *in = (const uint16_t *)inp;
    uint16_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
        *(out++) =  *(in++) + 0x8000;
}
