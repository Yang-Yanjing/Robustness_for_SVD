}
static void S20BDecode( void *outp, const uint8_t *in, unsigned samples )
{
    int32_t *out = outp;
    while( samples >= 2 )
    {
        uint32_t dw = U32_AT(in);
        in += 4;
        *(out++) = dw & ~0xFFF;
        *(out++) = (dw << 20) | (*in << 12);
        in++;
        samples -= 2;
    }
    /* No U32_AT() for the last odd sample: avoid off-by-one overflow! */
    if( samples )
        *(out++) = (U16_AT(in) << 16) | ((in[2] & 0xF0) << 8);
}
