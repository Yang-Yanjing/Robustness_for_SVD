/* For lines 0 [4] or 1 [4] */
static void Unpack01( const uint8_t *p_line, unsigned int i_size,
                      uint8_t *p_y, uint8_t *p_u, uint8_t *p_v )
{
    const uint8_t *p_end = p_line + i_size;
    while ( p_line < p_end )
    {
        *p_u++ = (U + 2) / 4;
        *p_y++ = (Y1 + 2) / 4;
        *p_v++ = (V + 2) / 4;
        *p_y++ = (Y2 + 2) / 4;
        p_line += 5;
    }
}
