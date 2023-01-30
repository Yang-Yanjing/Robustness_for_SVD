VLC_MMX
static int CalculateInterlaceScoreMMX( const picture_t* p_pic_top,
                                       const picture_t* p_pic_bot )
{
    assert( p_pic_top->i_planes == p_pic_bot->i_planes );
    /* Amount of bits must be known for MMX, thus int32_t.
       Doesn't hurt the C implementation. */
    int32_t i_score_mmx = 0; /* this must be divided by 255 when finished  */
    int32_t i_score_c   = 0; /* this counts as-is (used for non-MMX parts) */
    pxor_r2r( mm7, mm7 ); /* we will keep score in mm7 */
    for( int i_plane = 0 ; i_plane < p_pic_top->i_planes ; ++i_plane )
    {
        /* Sanity check */
        if( p_pic_top->p[i_plane].i_visible_lines !=
            p_pic_bot->p[i_plane].i_visible_lines )
            return -1;
        const int i_lasty = p_pic_top->p[i_plane].i_visible_lines-1;
        const int w = FFMIN( p_pic_top->p[i_plane].i_visible_pitch,
                             p_pic_bot->p[i_plane].i_visible_pitch );
        const int wm8 = w % 8;   /* remainder */
        const int w8  = w - wm8; /* part of width that is divisible by 8 */
        /* Current line / neighbouring lines picture pointers */
        const picture_t *cur = p_pic_bot;
        const picture_t *ngh = p_pic_top;
        int wc = cur->p[i_plane].i_pitch;
        int wn = ngh->p[i_plane].i_pitch;
        /* Transcode 1.1.5 only checks every other line. Checking every line
           works better for anime, which may contain horizontal,
           one pixel thick cartoon outlines.
        */
        for( int y = 1; y < i_lasty; ++y )
        {
            uint8_t *p_c = &cur->p[i_plane].p_pixels[y*wc];     /* this line */
            uint8_t *p_p = &ngh->p[i_plane].p_pixels[(y-1)*wn]; /* prev line */
            uint8_t *p_n = &ngh->p[i_plane].p_pixels[(y+1)*wn]; /* next line */
            int x = 0;
            /* Easy-to-read C version further below.
               Assumptions: 0 < T < 127
                            # of pixels < (2^32)/255
               Note: calculates score * 255
            */
            static const mmx_t b0   = { .uq = 0x0000000000000000ULL };
            static const mmx_t b128 = { .uq = 0x8080808080808080ULL };
            static const mmx_t bT   = { .ub = { T, T, T, T, T, T, T, T } };
            for( ; x < w8; x += 8 )
            {
                movq_m2r( *((int64_t*)p_c), mm0 );
                movq_m2r( *((int64_t*)p_p), mm1 );
                movq_m2r( *((int64_t*)p_n), mm2 );
                psubb_m2r( b128, mm0 );
                psubb_m2r( b128, mm1 );
                psubb_m2r( b128, mm2 );
                psubsb_r2r( mm0, mm1 );
                psubsb_r2r( mm0, mm2 );
                pxor_r2r( mm3, mm3 );
                pxor_r2r( mm4, mm4 );
                pxor_r2r( mm5, mm5 );
                pxor_r2r( mm6, mm6 );
                punpcklbw_r2r( mm1, mm3 );
                punpcklbw_r2r( mm2, mm4 );
                punpckhbw_r2r( mm1, mm5 );
                punpckhbw_r2r( mm2, mm6 );
                pmulhw_r2r( mm3, mm4 );
                pmulhw_r2r( mm5, mm6 );
                packsswb_r2r(mm4, mm6);
                pcmpgtb_m2r( bT, mm6 );
                psadbw_m2r( b0, mm6 );
                paddd_r2r( mm6, mm7 );
                p_c += 8;
                p_p += 8;
                p_n += 8;
            }
            for( ; x < w; ++x )
            {
                /* Worst case: need 17 bits for "comb". */
                int_fast32_t C = *p_c;
                int_fast32_t P = *p_p;
                int_fast32_t N = *p_n;
                /* Comments in Transcode's filter_ivtc.c attribute this
                   combing metric to Gunnar Thalin.
                    The idea is that if the picture is interlaced, both
                    expressions will have the same sign, and this comes
                    up positive. The value T = 100 has been chosen such
                    that a pixel difference of 10 (on average) will
                    trigger the detector.
                */
                int_fast32_t comb = (P - C) * (N - C);
                if( comb > T )
                    ++i_score_c;
                ++p_c;
                ++p_p;
                ++p_n;
            }
            /* Now the other field - swap current and neighbour pictures */
            const picture_t *tmp = cur;
            cur = ngh;
            ngh = tmp;
            int tmp_pitch = wc;
            wc = wn;
            wn = tmp_pitch;
        }
    }
    movd_r2m( mm7, i_score_mmx );
    emms();
    return i_score_mmx/255 + i_score_c;
}
