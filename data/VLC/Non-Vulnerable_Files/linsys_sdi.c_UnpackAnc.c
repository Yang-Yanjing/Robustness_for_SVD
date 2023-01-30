#define A3  (uint16_t)((p_anc[3] >> 6) | (p_anc[4] << 2))
static void UnpackAnc( const uint8_t *p_anc, unsigned int i_size,
                       uint16_t *p_dest )
{
    const uint8_t *p_end = p_anc + i_size;
    while ( p_anc <= p_end - 5 )
    {
        *p_dest++ = A0;
        *p_dest++ = A1;
        *p_dest++ = A2;
        *p_dest++ = A3;
        p_anc += 5;
    }
}
