}
static void F64IDecode( void *outp, const uint8_t *in, unsigned samples )
{
    double *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        union { double d; uint64_t u; } s;
#ifdef WORDS_BIGENDIAN
        s.u = GetQWLE( in );
#else
        s.u = GetQWBE( in );
#endif
        if( unlikely(!isfinite( s.d )) )
            s.d = 0.;
        *(out++) = s.d;
        in += 8;
    }
}
