/* For lines 0 [4] or 1 [4] */
static void Unpack01( const uint8_t *p_line, unsigned int i_size,
                      uint8_t *p_y, uint8_t *p_u, uint8_t *p_v )
{
    const uint8_t *p_end = p_line + i_size;
    while ( p_line < p_end )
    {
        *p_u++ = U;
        *p_y++ = Y1;
        *p_v++ = V;
        *p_y++ = Y2;
        p_line += 4;
    }
}
