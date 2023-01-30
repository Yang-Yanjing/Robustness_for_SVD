 *****************************************************************************/
int puzzle_bake_piece( filter_t *p_filter)
{
    int i_ret = puzzle_allocate_ps_pieces( p_filter);
    if (i_ret != VLC_SUCCESS)
        return i_ret;
    filter_sys_t *p_sys = p_filter->p_sys;
    /* generates random pi_order array */
    i_ret = puzzle_shuffle( p_filter );
    if (i_ret != VLC_SUCCESS)
        return i_ret;
    int32_t i = 0;
    for (int32_t row = 0; row < p_sys->s_allocated.i_rows; row++) {
        for (int32_t col = 0; col < p_sys->s_allocated.i_cols; col++) {
            int32_t orow = row;
            int32_t ocol = col;
            if (p_sys->pi_order != NULL) {
                orow = p_sys->pi_order[i] / (p_sys->s_allocated.i_cols);
                ocol = p_sys->pi_order[i] % (p_sys->s_allocated.i_cols);
            }
            p_sys->ps_pieces[i].i_original_row = orow;
            p_sys->ps_pieces[i].i_original_col = ocol;
            /* set bottom and right shapes */
            p_sys->ps_pieces[i].i_left_shape  = 0;
            p_sys->ps_pieces[i].i_top_shape   = 2;
            p_sys->ps_pieces[i].i_btm_shape   = 4;
            p_sys->ps_pieces[i].i_right_shape = 6;
            if (p_sys->s_allocated.i_shape_size > 0) {
                if (orow < p_sys->s_allocated.i_rows - 1)
                    p_sys->ps_pieces[i].i_btm_shape = 4 + 8 + 8*(( (unsigned) vlc_mrand48()) % ( SHAPES_QTY ) ) + (vlc_mrand48() & 0x01);
                if (ocol < p_sys->s_allocated.i_cols - 1)
                    p_sys->ps_pieces[i].i_right_shape = 6 + 8 + 8*(( (unsigned) vlc_mrand48()) % ( SHAPES_QTY ) ) + (vlc_mrand48() & 0x01);
            }
            /* set piece data */
            p_sys->ps_pieces[i].i_actual_angle   = 0;
            p_sys->ps_pieces[i].b_overlap        = false;
            p_sys->ps_pieces[i].i_actual_mirror  = +1;
            p_sys->ps_pieces[i].b_finished       = ((ocol == col) && (orow == row));
            p_sys->ps_pieces[i].i_group_ID       = i;
            /* add small random offset to location */
            int32_t i_rand_x = 0;
            int32_t i_rand_y = 0;
            if (p_sys->s_current_param.b_advanced) {
                i_rand_x = (( (unsigned) vlc_mrand48()) % ( p_sys->ps_desk_planes[0].i_pce_max_width + 1 ) ) - (int32_t) p_sys->ps_desk_planes[0].i_pce_max_width / 2;
                i_rand_y = (( (unsigned) vlc_mrand48()) % ( p_sys->ps_desk_planes[0].i_pce_max_lines + 1 ) ) - (int32_t) p_sys->ps_desk_planes[0].i_pce_max_lines / 2;
            }
            /* copy related puzzle data to piece data */
            if (p_sys->ps_puzzle_array != NULL) {
                for (uint8_t i_plane = 0; i_plane < p_sys->s_allocated.i_planes; i_plane++) {
                    p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_width = p_sys->ps_puzzle_array[row][col][i_plane].i_width;
                    p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_lines = p_sys->ps_puzzle_array[row][col][i_plane].i_lines;
                    p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_original_x = p_sys->ps_puzzle_array[orow][ocol][i_plane].i_x;
                    p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_original_y = p_sys->ps_puzzle_array[orow][ocol][i_plane].i_y;
                    p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_actual_x = p_sys->ps_puzzle_array[row][col][i_plane].i_x + i_rand_x *
                        p_sys->ps_desk_planes[i_plane].i_width / p_sys->ps_desk_planes[0].i_width;
                    p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_actual_y = p_sys->ps_puzzle_array[row][col][i_plane].i_y + i_rand_y *
                        p_sys->ps_desk_planes[i_plane].i_lines / p_sys->ps_desk_planes[0].i_lines;
                    if (i_plane == 0) {
                        p_sys->ps_pieces[i].i_OLx = p_sys->ps_pieces[i].ps_piece_in_plane[0].i_original_x;
                        p_sys->ps_pieces[i].i_OTy = p_sys->ps_pieces[i].ps_piece_in_plane[0].i_original_y;
                        p_sys->ps_pieces[i].i_ORx = p_sys->ps_pieces[i].ps_piece_in_plane[0].i_original_x + p_sys->ps_pieces[i].ps_piece_in_plane[0].i_width - 1;
                        p_sys->ps_pieces[i].i_OBy = p_sys->ps_pieces[i].ps_piece_in_plane[0].i_original_y + p_sys->ps_pieces[i].ps_piece_in_plane[0].i_lines - 1;
                        puzzle_calculate_corners( p_filter, i );
                    }
                }
            }
            i++;
        }
    }
    /* left and top shapes are based on negative right and bottom ones */
    puzzle_set_left_top_shapes( p_filter);
    /* add random rotation to each piece */
    puzzle_random_rotate( p_filter);
    return VLC_SUCCESS;
}
