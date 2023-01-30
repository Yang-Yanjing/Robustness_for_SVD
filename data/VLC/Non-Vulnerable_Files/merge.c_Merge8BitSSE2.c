VLC_SSE
void Merge8BitSSE2( void *_p_dest, const void *_p_s1, const void *_p_s2,
                    size_t i_bytes )
{
    uint8_t *p_dest = _p_dest;
    const uint8_t *p_s1 = _p_s1;
    const uint8_t *p_s2 = _p_s2;
    for( ; i_bytes > 0 && ((uintptr_t)p_s1 & 15); i_bytes-- )
        *p_dest++ = ( *p_s1++ + *p_s2++ ) >> 1;
    for( ; i_bytes >= 16; i_bytes -= 16 )
    {
        __asm__  __volatile__( "movdqu %2,%%xmm1;"
                               "pavgb %1, %%xmm1;"
                               "movdqu %%xmm1, %0" :"=m" (*p_dest):
                                                 "m" (*p_s1),
                                                 "m" (*p_s2) : "xmm1" );
        p_dest += 16;
        p_s1 += 16;
        p_s2 += 16;
    }
    for( ; i_bytes > 0; i_bytes-- )
        *p_dest++ = ( *p_s1++ + *p_s2++ ) >> 1;
}
