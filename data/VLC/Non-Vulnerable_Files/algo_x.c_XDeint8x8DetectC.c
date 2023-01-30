static inline int ssd( int a ) { return a*a; }
static inline int XDeint8x8DetectC( uint8_t *src, int i_src )
{
    int y, x;
    int ff, fr;
    int fc;
    /* Detect interlacing */
    fc = 0;
    for( y = 0; y < 7; y += 2 )
    {
        ff = fr = 0;
        for( x = 0; x < 8; x++ )
        {
            fr += ssd(src[      x] - src[1*i_src+x]) +
                  ssd(src[i_src+x] - src[2*i_src+x]);
            ff += ssd(src[      x] - src[2*i_src+x]) +
                  ssd(src[i_src+x] - src[3*i_src+x]);
        }
        if( ff < 6*fr/8 && fr > 32 )
            fc++;
        src += 2*i_src;
    }
    return fc < 1 ? false : true;
}
