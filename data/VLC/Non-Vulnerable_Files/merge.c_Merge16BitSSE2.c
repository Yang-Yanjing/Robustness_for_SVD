VLC_SSE
void Merge16BitSSE2( void *_p_dest, const void *_p_s1, const void *_p_s2,
                     size_t i_bytes )
{
    uint16_t *p_dest = _p_dest;
    const uint16_t *p_s1 = _p_s1;
    const uint16_t *p_s2 = _p_s2;
    size_t i_words = i_bytes / 2;
    for( ; i_words > 0 && ((uintptr_t)p_s1 & 15); i_words-- )
        *p_dest++ = ( *p_s1++ + *p_s2++ ) >> 1;
    for( ; i_words >= 8; i_words -= 8 )
    {
        __asm__  __volatile__( "movdqu %2,%%xmm1;"
                               "pavgw %1, %%xmm1;"
                               "movdqu %%xmm1, %0" :"=m" (*p_dest):
                                                 "m" (*p_s1),
                                                 "m" (*p_s2) : "xmm1" );
        p_dest += 8;
        p_s1 += 8;
        p_s2 += 8;
    }
    for( ; i_words > 0; i_words-- )
        *p_dest++ = ( *p_s1++ + *p_s2++ ) >> 1;
}
