 */
static int Configuration( panoramix_output_t pp_output[ROW_MAX][COL_MAX],
                          int i_col, int i_row,
                          int i_src_width, int i_src_height,
                          int i_half_w, int i_half_h,
                          bool b_attenuate,
                          const bool *pb_active )
{
#ifdef OVERLAP
    const bool b_overlap = true;
#else
    const bool b_overlap = false;
#endif
    /* */
    int i_output = 0;
    for( int y = 0, i_src_y = 0, i_dst_y = 0; y < i_row; y++ )
    {
        const bool b_row_first = y == 0;
        const bool b_row_last  = y == i_row - 1;
        /* Compute source height */
        int i_win_height = (i_src_height / i_row ) & ~1;
        if( b_row_last )
            i_win_height = i_src_height - y * i_win_height;
        for( int x = 0, i_src_x = 0, i_dst_x = 0; x < i_col; x++ )
        {
            const bool b_col_first = x == 0;
            const bool b_col_last  = x == i_col - 1;
            /* Compute source width */
            int i_win_width  = (i_src_width  / i_col ) & ~1;
            if( b_col_last )
                i_win_width  = i_src_width  - x * i_win_width;
            /* Compute filter configuration */
            panoramix_filter_t cfg;
            memset( &cfg, 0, sizeof(cfg) );
            if( b_overlap && b_attenuate )
            {
                if( i_col > 2 )
                {
                    if( b_col_first )
                        cfg.black.i_left   = i_half_w;
                    if( b_col_last )
                        cfg.black.i_right  = i_half_w;
                }
                if( i_row > 2 )
                {
                    if( b_row_first )
                        cfg.black.i_top    = i_half_h;
                    if( b_row_last )
                        cfg.black.i_bottom = i_half_h;
                }
                if( !b_col_first )
                    cfg.attenuate.i_left   = 2 * i_half_w;
                if( !b_col_last )
                    cfg.attenuate.i_right  = 2 * i_half_w;
                if( !b_row_first )
                    cfg.attenuate.i_top    = 2 * i_half_h;
                if( !b_row_last )
                    cfg.attenuate.i_bottom = 2 * i_half_h;
            }
            /* Compute alignment */
            int i_align = 0;
            if( i_col > 1 )
            {
                if( b_col_first )
                    i_align |= VOUT_ALIGN_RIGHT;
                if( b_col_last )
                    i_align |= VOUT_ALIGN_LEFT;
            }
            if( i_row > 1 )
            {
                if( b_row_first )
                    i_align |= VOUT_ALIGN_BOTTOM;
                if( b_row_last )
                    i_align |= VOUT_ALIGN_TOP;
            }
            /* */
            panoramix_output_t *p_output = &pp_output[x][y];
            /* */
            p_output->i_src_x = i_src_x - cfg.attenuate.i_left/2;
            p_output->i_src_y = i_src_y - cfg.attenuate.i_top/2;
            p_output->i_src_width  = i_win_width  + cfg.attenuate.i_left/2 + cfg.attenuate.i_right/2;
            p_output->i_src_height = i_win_height + cfg.attenuate.i_top/2  + cfg.attenuate.i_bottom/2;
            /* */
            p_output->filter = cfg;
            /* */
            p_output->i_align = i_align;
            p_output->i_x = i_dst_x;
            p_output->i_y = i_dst_y;
            p_output->i_width  = cfg.black.i_left + p_output->i_src_width  + cfg.black.i_right;
            p_output->i_height = cfg.black.i_top  + p_output->i_src_height + cfg.black.i_bottom;
            /* */
            p_output->b_active = pb_active[y * i_col + x] &&
                                 p_output->i_width > 0 &&
                                 p_output->i_height > 0;
            if( p_output->b_active )
                p_output->i_output = i_output++;
            /* */
            i_src_x += i_win_width;
            i_dst_x += p_output->i_width;
        }
        i_src_y += i_win_height;
        i_dst_y += pp_output[0][y].i_height;
    }
    return i_output;
}
