 *****************************************************************************/
static void Exchange( sample_t *restrict p_out, const sample_t *restrict p_in )
{
    const sample_t *p_first = p_in + 256;
    const sample_t *p_second = p_in;
    for( unsigned i = 0; i < 256; i++ )
    {
#ifdef LIBA52_FIXED
        uint32_t spl[2];
        spl[0] = ((uint32_t)*p_first++) << 4;
        spl[1] = ((uint32_t)*p_second++) << 4;
        memcpy( p_out, spl, sizeof(spl) );
        p_out += 2;
#else
        *p_out++ = *p_first++;
        *p_out++ = *p_second++;
#endif
    }
}
