#define Y2  (uint16_t)((p_line[3] >> 6) | (p_line[4] << 2))
static void UnpackVBI( const uint8_t *p_line, unsigned int i_size,
                       uint8_t *p_dest )
{
    const uint8_t *p_end = p_line + i_size;
    while ( p_line < p_end )
    {
        *p_dest++ = (U + 2) / 4;
        *p_dest++ = (Y1 + 2) / 4;
        *p_dest++ = (V + 2) / 4;
        *p_dest++ = (Y2 + 2) / 4;
        p_line += 5;
    }
}
