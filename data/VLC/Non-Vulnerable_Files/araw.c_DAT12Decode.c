}
static void DAT12Decode( void *outp, const uint8_t *in, unsigned samples )
{
    int32_t *out = outp;
    while( samples >= 2 )
    {
        *(out++) = dat12tos16(U16_AT(in) >> 4);
        *(out++) = dat12tos16(U16_AT(in + 1) & ~0xF000);
        in += 3;
        samples -= 2;
    }
    if( samples )
        *(out++) = dat12tos16(U16_AT(in) >> 4);
}
