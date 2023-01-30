}
static void F32IDecode( void *outp, const uint8_t *in, unsigned samples )
{
    float *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        union { float f; uint32_t u; } s;
#ifdef WORDS_BIGENDIAN
        s.u = GetDWLE( in );
#else
        s.u = GetDWBE( in );
#endif
        if( unlikely(!isfinite(s.f)) )
            s.f = 0.f;
        *(out++) = s.f;
        in += 4;
    }
}
