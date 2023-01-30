}
static void F32IEncode( void *outp, const uint8_t *inp, unsigned samples )
{
    const float *in = (const float *)inp;
    uint8_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        union { float f; uint32_t u; char b[4]; } s;
        s.f = *(in++);
        s.u = bswap32( s.u );
        memcpy( out, s.b, 4 );
        out += 4;
    }
}
