 *****************************************************************************/
static void Duplicate( sample_t *restrict p_out, const sample_t *restrict p_in )
{
    for( unsigned i = 256; i--; )
    {
#ifdef LIBA52_FIXED
        union { uint32_t u; int32_t i; } spl;
        spl.u = ((uint32_t)*(p_in++)) << 4;
        *p_out++ = spl.i;
        *p_out++ = spl.i;
#else
        sample_t s = *(p_in++);
        *p_out++ = s;
        *p_out++ = s;
#endif
    }
}
