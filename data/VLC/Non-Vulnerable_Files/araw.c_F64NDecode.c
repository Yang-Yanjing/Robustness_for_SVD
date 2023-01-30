}
static void F64NDecode( void *outp, const uint8_t *in, unsigned samples )
{
    double *out = outp;
    for( size_t i = 0; i < samples; i++ )
    {
        memcpy( out, in, sizeof(double) );
        if( unlikely(!isfinite( *out )) )
            *out = 0.;
        out++;
        in += sizeof(double);
    }
}
