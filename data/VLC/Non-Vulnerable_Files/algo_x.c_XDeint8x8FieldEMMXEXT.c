VLC_MMX
static inline void XDeint8x8FieldEMMXEXT( uint8_t *dst, int i_dst,
                                          uint8_t *src, int i_src )
{
    int y;
    /* Interlaced */
    for( y = 0; y < 8; y += 2 )
    {
        movq_m2r( src[0], mm0 );
        movq_r2m( mm0, dst[0] );
        dst += i_dst;
        movq_m2r( src[2*i_src], mm1 );
        pavgb_r2r( mm1, mm0 );
        movq_r2m( mm0, dst[0] );
        dst += 1*i_dst;
        src += 2*i_src;
    }
}
