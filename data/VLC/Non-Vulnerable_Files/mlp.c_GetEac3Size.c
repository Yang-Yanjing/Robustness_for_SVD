 */
static int GetEac3Size( const uint8_t *p_buf )
{
    int i_frame_size;
    int i_bytes;
    i_frame_size = ( ( p_buf[2] << 8 ) | p_buf[3] ) & 0x7ff;
    if( i_frame_size < 2 )
        return 0;
    i_bytes = 2 * ( i_frame_size + 1 );
    return i_bytes;
}
