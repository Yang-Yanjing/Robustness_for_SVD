}
static void F64IEncode( void *outp, const uint8_t *inp, unsigned samples )
{
    const double *in = (const double *)inp;
    uint8_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        union { double d; uint64_t u; char b[8]; } s;
        s.d = *(in++);
        s.u = bswap64( s.u );
        memcpy( out, s.b, 8 );
        out += 8;
    }
}
