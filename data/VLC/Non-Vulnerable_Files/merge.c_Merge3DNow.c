VLC_MMX
void Merge3DNow( void *_p_dest, const void *_p_s1, const void *_p_s2,
                 size_t i_bytes )
{
    uint8_t *p_dest = _p_dest;
    const uint8_t *p_s1 = _p_s1;
    const uint8_t *p_s2 = _p_s2;
    for( ; i_bytes >= 8; i_bytes -= 8 )
    {
        __asm__  __volatile__( "movq %2,%%mm1;"
                               "pavgusb %1, %%mm1;"
                               "movq %%mm1, %0" :"=m" (*p_dest):
                                                 "m" (*p_s1),
                                                 "m" (*p_s2) : "mm1" );
        p_dest += 8;
        p_s1 += 8;
        p_s2 += 8;
    }
    for( ; i_bytes > 0; i_bytes-- )
        *p_dest++ = ( *p_s1++ + *p_s2++ ) >> 1;
}
