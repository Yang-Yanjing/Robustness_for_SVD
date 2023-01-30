}
static void S8Decode( void *outp, const uint8_t *in, unsigned samples )
{
    uint8_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
        out[i] = in[i] ^ 0x80;
}
