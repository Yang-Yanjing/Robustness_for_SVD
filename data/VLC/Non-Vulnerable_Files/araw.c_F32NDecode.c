}
static void F32NDecode( void *outp, const uint8_t *in, unsigned samples )
{
    float *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        memcpy( out, in, sizeof(float) );
        if( unlikely(!isfinite(*out)) )
            *out = 0.f;
        out++;
        in += sizeof(float);
    }
}
