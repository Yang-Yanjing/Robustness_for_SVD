VLC_MMX
static inline int XDeint8x8DetectMMXEXT( uint8_t *src, int i_src )
{
    int y, x;
    int32_t ff, fr;
    int fc;
    /* Detect interlacing */
    fc = 0;
    pxor_r2r( mm7, mm7 );
    for( y = 0; y < 9; y += 2 )
    {
        ff = fr = 0;
        pxor_r2r( mm5, mm5 );
        pxor_r2r( mm6, mm6 );
        for( x = 0; x < 8; x+=4 )
        {
            movd_m2r( src[        x], mm0 );
            movd_m2r( src[1*i_src+x], mm1 );
            movd_m2r( src[2*i_src+x], mm2 );
            movd_m2r( src[3*i_src+x], mm3 );
            punpcklbw_r2r( mm7, mm0 );
            punpcklbw_r2r( mm7, mm1 );
            punpcklbw_r2r( mm7, mm2 );
            punpcklbw_r2r( mm7, mm3 );
            movq_r2r( mm0, mm4 );
            psubw_r2r( mm1, mm0 );
            psubw_r2r( mm2, mm4 );
            psubw_r2r( mm1, mm2 );
            psubw_r2r( mm1, mm3 );
            pmaddwd_r2r( mm0, mm0 );
            pmaddwd_r2r( mm4, mm4 );
            pmaddwd_r2r( mm2, mm2 );
            pmaddwd_r2r( mm3, mm3 );
            paddd_r2r( mm0, mm2 );
            paddd_r2r( mm4, mm3 );
            paddd_r2r( mm2, mm5 );
            paddd_r2r( mm3, mm6 );
        }
        movq_r2r( mm5, mm0 );
        psrlq_i2r( 32, mm0 );
        paddd_r2r( mm0, mm5 );
        movd_r2m( mm5, fr );
        movq_r2r( mm6, mm0 );
        psrlq_i2r( 32, mm0 );
        paddd_r2r( mm0, mm6 );
        movd_r2m( mm6, ff );
        if( ff < 6*fr/8 && fr > 32 )
            fc++;
        src += 2*i_src;
    }
    return fc;
}
