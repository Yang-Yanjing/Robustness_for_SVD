 */
static void DarkenField( picture_t *p_dst,
                         const int i_field, const int i_strength,
                         bool process_chroma )
{
    assert( p_dst != NULL );
    assert( i_field == 0 || i_field == 1 );
    assert( i_strength >= 1 && i_strength <= 3 );
    /* Bitwise ANDing with this clears the i_strength highest bits
       of each byte */
    const uint8_t  remove_high_u8 = 0xFF >> i_strength;
    const uint64_t remove_high_u64 = remove_high_u8 *
                                            INT64_C(0x0101010101010101);
    /* Process luma.
       For luma, the operation is just a shift + bitwise AND, so we vectorize
       even in the C version.
       There is an MMX version too, because it performs about twice faster.
    */
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
        for( ; x < w8; x += 8, ++po )
            (*po) = ( ((*po) >> i_strength) & remove_high_u64 );
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
        for( i_plane++ /* luma already handled*/;
             i_plane < p_dst->i_planes;
             i_plane++ )
        {
            int w = p_dst->p[i_plane].i_visible_pitch;
            p_out = p_dst->p[i_plane].p_pixels;
            p_out_end = p_out + p_dst->p[i_plane].i_pitch
                              * p_dst->p[i_plane].i_visible_lines;
            /* skip first line for bottom field */
            if( i_field == 1 )
                p_out += p_dst->p[i_plane].i_pitch;
            for( ; p_out < p_out_end ; p_out += 2*p_dst->p[i_plane].i_pitch )
            {
                /* Handle the width remainder */
                uint8_t *po = p_out;
                for( int x = 0; x < w; ++x, ++po )
                    (*po) = 128 + ( ((*po) - 128) / (1 << i_strength) );
            } /* for p_out... */
        } /* for i_plane... */
    } /* if process_chroma */
}
