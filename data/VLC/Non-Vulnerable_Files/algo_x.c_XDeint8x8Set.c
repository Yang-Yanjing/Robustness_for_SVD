/* For debug */
static inline void XDeint8x8Set( uint8_t *dst, int i_dst, uint8_t v )
{
    int y;
    for( y = 0; y < 8; y++ )
        memset( &dst[y*i_dst], v, 8 );
}
