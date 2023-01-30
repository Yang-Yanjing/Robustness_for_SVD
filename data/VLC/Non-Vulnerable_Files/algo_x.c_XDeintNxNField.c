}
static inline void XDeintNxNField( uint8_t *dst, int i_dst,
                                   uint8_t *src, int i_src,
                                   int i_width, int i_height )
{
    int y, x;
    /* Interlaced */
    for( y = 0; y < i_height; y += 2 )
    {
        memcpy( dst, src, i_width );
        dst += i_dst;
        if( y < i_height - 2 )
        {
            for( x = 0; x < i_width; x++ )
                dst[x] = (src[x] + src[2*i_src+x] ) >> 1;
        }
        else
        {
            /* Blend last line */
            for( x = 0; x < i_width; x++ )
                dst[x] = (src[x] + src[i_src+x]) >> 1;
        }
        dst += 1*i_dst;
        src += 2*i_src;
    }
}
}
static inline void XDeintNxNField( uint8_t *dst, int i_dst,
                                   uint8_t *src, int i_src,
                                   int i_width, int i_height )
{
    int y, x;
    /* Interlaced */
    for( y = 0; y < i_height; y += 2 )
    {
        memcpy( dst, src, i_width );
        dst += i_dst;
        if( y < i_height - 2 )
        {
            for( x = 0; x < i_width; x++ )
                dst[x] = (src[x] + src[2*i_src+x] ) >> 1;
        }
        else
        {
            /* Blend last line */
            for( x = 0; x < i_width; x++ )
                dst[x] = (src[x] + src[i_src+x]) >> 1;
        }
        dst += 1*i_dst;
        src += 2*i_src;
    }
