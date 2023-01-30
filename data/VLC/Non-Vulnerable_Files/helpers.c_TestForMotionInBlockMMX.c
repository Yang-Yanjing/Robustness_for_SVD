VLC_MMX
static int TestForMotionInBlockMMX( uint8_t *p_pix_p, uint8_t *p_pix_c,
                                    int i_pitch_prev, int i_pitch_curr,
                                    int* pi_top, int* pi_bot )
{
    int32_t i_motion = 0;
    int32_t i_top_motion = 0;
    int32_t i_bot_motion = 0;
    static const mmx_t bT   = { .ub = { T, T, T, T, T, T, T, T } };
    pxor_r2r( mm6, mm6 ); /* zero, used in psadbw */
    movq_m2r( bT,  mm5 );
    pxor_r2r( mm3, mm3 ); /* score (top field) */
    pxor_r2r( mm4, mm4 ); /* score (bottom field) */
    for( int y = 0; y < 8; y+=2 )
    {
        /* top field */
        movq_m2r( *((uint64_t*)p_pix_c), mm0 );
        movq_m2r( *((uint64_t*)p_pix_p), mm1 );
        movq_r2r( mm0, mm2 );
        psubusb_r2r( mm1, mm2 );
        psubusb_r2r( mm0, mm1 );
        pcmpgtb_r2r( mm5, mm2 );
        pcmpgtb_r2r( mm5, mm1 );
        psadbw_r2r(  mm6, mm2 );
        psadbw_r2r(  mm6, mm1 );
        paddd_r2r( mm2, mm1 );
        paddd_r2r( mm1, mm3 ); /* add to top field score */
        p_pix_c += i_pitch_curr;
        p_pix_p += i_pitch_prev;
        /* bottom field - handling identical to top field, except... */
        movq_m2r( *((uint64_t*)p_pix_c), mm0 );
        movq_m2r( *((uint64_t*)p_pix_p), mm1 );
        movq_r2r( mm0, mm2 );
        psubusb_r2r( mm1, mm2 );
        psubusb_r2r( mm0, mm1 );
        pcmpgtb_r2r( mm5, mm2 );
        pcmpgtb_r2r( mm5, mm1 );
        psadbw_r2r(  mm6, mm2 );
        psadbw_r2r(  mm6, mm1 );
        paddd_r2r( mm2, mm1 );
        paddd_r2r( mm1, mm4 ); /* ...here we add to bottom field score */
        p_pix_c += i_pitch_curr;
        p_pix_p += i_pitch_prev;
    }
    movq_r2r(  mm3, mm7 ); /* score (total) */
    paddd_r2r( mm4, mm7 );
    movd_r2m( mm3, i_top_motion );
    movd_r2m( mm4, i_bot_motion );
    movd_r2m( mm7, i_motion );
    /* The loop counts actual score * 255. */
    i_top_motion /= 255;
    i_bot_motion /= 255;
    i_motion     /= 255;
    emms();
    (*pi_top) = ( i_top_motion >= 8 );
    (*pi_bot) = ( i_bot_motion >= 8 );
    return (i_motion >= 8);
}
