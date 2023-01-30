VLC_MMX
static void DarkenFieldMMX( picture_t *p_dst,
                            const int i_field, const int i_strength,
                            bool process_chroma )
{
    assert( p_dst != NULL );
    assert( i_field == 0 || i_field == 1 );
    assert( i_strength >= 1 && i_strength <= 3 );
    uint64_t i_strength_u64 = i_strength; /* needs to know number of bits */
    const uint8_t  remove_high_u8 = 0xFF >> i_strength;
    const uint64_t remove_high_u64 = remove_high_u8 *
                                            INT64_C(0x0101010101010101);
    int i_plane = Y_PLANE;
    uint8_t *p_out, *p_out_end;
    int w = p_dst->p[i_plane].i_visible_pitch;
    p_out = p_dst->p[i_plane].p_pixels;
    p_out_end = p_out + p_dst->p[i_plane].i_pitch
                      * p_dst->p[i_plane].i_visible_lines;
    /* skip first line for bottom field */
    if( i_field == 1 )
        p_out += p_dst->p[i_plane].i_pitch;
    int wm8 = w % 8;   /* remainder */
    int w8  = w - wm8; /* part of width that is divisible by 8 */
    for( ; p_out < p_out_end ; p_out += 2*p_dst->p[i_plane].i_pitch )
    {
        uint64_t *po = (uint64_t *)p_out;
        int x = 0;
        movq_m2r( i_strength_u64,  mm1 );
        movq_m2r( remove_high_u64, mm2 );
        for( ; x < w8; x += 8 )
        {
            movq_m2r( (*po), mm0 );
            psrlq_r2r( mm1, mm0 );
            pand_r2r(  mm2, mm0 );
            movq_r2m( mm0, (*po++) );
        }
        /* handle the width remainder */
        uint8_t *po_temp = (uint8_t *)po;
        for( ; x < w; ++x, ++po_temp )
            (*po_temp) = ( ((*po_temp) >> i_strength) & remove_high_u8 );
    }
    /* Process chroma if the field chromas are independent.
       The origin (black) is at YUV = (0, 128, 128) in the uint8 format.
       The chroma processing is a bit more complicated than luma,
       and needs MMX for vectorization.
    */
    if( process_chroma )
    {
        for( i_plane++ /* luma already handled */;
             i_plane < p_dst->i_planes;
             i_plane++ )
        {
            int w = p_dst->p[i_plane].i_visible_pitch;
            int wm8 = w % 8;   /* remainder */
            int w8  = w - wm8; /* part of width that is divisible by 8 */
            p_out = p_dst->p[i_plane].p_pixels;
            p_out_end = p_out + p_dst->p[i_plane].i_pitch
                              * p_dst->p[i_plane].i_visible_lines;
            /* skip first line for bottom field */
            if( i_field == 1 )
                p_out += p_dst->p[i_plane].i_pitch;
            for( ; p_out < p_out_end ; p_out += 2*p_dst->p[i_plane].i_pitch )
            {
                int x = 0;
                /* See also easy-to-read C version below. */
                static const mmx_t b128 = { .uq = 0x8080808080808080ULL };
                movq_m2r( b128, mm5 );
                movq_m2r( i_strength_u64,  mm6 );
                movq_m2r( remove_high_u64, mm7 );
                uint64_t *po8 = (uint64_t *)p_out;
                for( ; x < w8; x += 8 )
                {
                    movq_m2r( (*po8), mm0 );
                    movq_r2r( mm5, mm2 ); /* 128 */
                    movq_r2r( mm0, mm1 ); /* copy of data */
                    psubusb_r2r( mm2, mm1 ); /* mm1 = max(data - 128, 0) */
                    psubusb_r2r( mm0, mm2 ); /* mm2 = max(128 - data, 0) */
                    /* >> i_strength */
                    psrlq_r2r( mm6, mm1 );
                    psrlq_r2r( mm6, mm2 );
                    pand_r2r(  mm7, mm1 );
                    pand_r2r(  mm7, mm2 );
                    /* collect results from pos./neg. parts */
                    psubb_r2r( mm2, mm1 );
                    paddb_r2r( mm5, mm1 );
                    movq_r2m( mm1, (*po8++) );
                }
                /* C version - handle the width remainder */
                uint8_t *po = p_out;
                for( ; x < w; ++x, ++po )
                    (*po) = 128 + ( ((*po) - 128) / (1 << i_strength) );
            } /* for p_out... */
        } /* for i_plane... */
    } /* if process_chroma */
    emms();
}
