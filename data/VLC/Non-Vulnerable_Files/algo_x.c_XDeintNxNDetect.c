 */
static inline int XDeintNxNDetect( uint8_t *src, int i_src,
                                   int i_height, int i_width )
{
    int y, x;
    int ff, fr;
    int fc;
    /* Detect interlacing */
    /* FIXME way too simple, need to be more like XDeint8x8Detect */
    ff = fr = 0;
    fc = 0;
    for( y = 0; y < i_height - 2; y += 2 )
    {
        const uint8_t *s = &src[y*i_src];
        for( x = 0; x < i_width; x++ )
        {
            fr += ssd(s[      x] - s[1*i_src+x]);
            ff += ssd(s[      x] - s[2*i_src+x]);
        }
        if( ff < fr && fr > i_width / 2 )
            fc++;
    }
    return fc < 2 ? false : true;
}
