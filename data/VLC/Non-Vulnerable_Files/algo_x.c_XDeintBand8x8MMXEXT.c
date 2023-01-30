VLC_MMX
static inline void XDeintBand8x8MMXEXT( uint8_t *dst, int i_dst,
                                        uint8_t *src, int i_src,
                                        const int i_mbx, int i_modx )
{
    int x;
    /* Reset current line */
    for( x = 0; x < i_mbx; x++ )
    {
        int s;
        if( ( s = XDeint8x8DetectMMXEXT( src, i_src ) ) )
        {
            if( x == 0 || x == i_mbx - 1 )
                XDeint8x8FieldEMMXEXT( dst, i_dst, src, i_src );
            else
                XDeint8x8FieldMMXEXT( dst, i_dst, src, i_src );
        }
        else
        {
            XDeint8x8MergeMMXEXT( dst, i_dst,
                                  &src[0*i_src], 2*i_src,
                                  &src[1*i_src], 2*i_src );
        }
        dst += 8;
        src += 8;
    }
    if( i_modx )
        XDeintNxN( dst, i_dst, src, i_src, i_modx, 8 );
}
VLC_MMX
static inline void XDeintBand8x8MMXEXT( uint8_t *dst, int i_dst,
                                        uint8_t *src, int i_src,
                                        const int i_mbx, int i_modx )
{
    int x;
    /* Reset current line */
    for( x = 0; x < i_mbx; x++ )
    {
        int s;
        if( ( s = XDeint8x8DetectMMXEXT( src, i_src ) ) )
        {
            if( x == 0 || x == i_mbx - 1 )
                XDeint8x8FieldEMMXEXT( dst, i_dst, src, i_src );
            else
                XDeint8x8FieldMMXEXT( dst, i_dst, src, i_src );
        }
        else
        {
            XDeint8x8MergeMMXEXT( dst, i_dst,
                                  &src[0*i_src], 2*i_src,
                                  &src[1*i_src], 2*i_src );
        }
        dst += 8;
        src += 8;
    }
