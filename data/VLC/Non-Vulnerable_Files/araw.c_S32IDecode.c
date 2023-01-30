}
static void S32IDecode( void *outp, const uint8_t *in, unsigned samples )
{
    int32_t *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
#ifdef WORDS_BIGENDIAN
        *(out++) = GetDWLE( in );
#else
        *(out++) = GetDWBE( in );
#endif
        in += 4;
    }
}
