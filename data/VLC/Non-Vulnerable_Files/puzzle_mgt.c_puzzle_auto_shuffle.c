 *****************************************************************************/
void puzzle_auto_shuffle( filter_t *p_filter)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if ( p_sys->s_current_param.i_auto_shuffle_speed < 500 )
        return;
    if ( --p_sys->i_auto_shuffle_countdown_val > 0 )
        return;
    /* delay reached, preset next delay and proceed with puzzle_auto_shuffle */
    p_sys->i_auto_shuffle_countdown_val = init_countdown(p_sys->s_current_param.i_auto_shuffle_speed);
    /* random piece to be moved */
    int32_t i_start = ((unsigned)vlc_mrand48()) % p_sys->s_allocated.i_pieces_nbr;
    for (uint32_t i_l = 0; i_l < p_sys->s_allocated.i_pieces_nbr; i_l++){
        int32_t i = ( i_l + i_start ) % p_sys->s_allocated.i_pieces_nbr;
        /* find one piece which is part of one group */
        if ( p_sys->pi_group_qty[p_sys->ps_pieces[i].i_group_ID] > 1 ) {
            /* find an empty group to be used by this dismantled piece */
            uint32_t i_new_group;
            for ( i_new_group = 0 ; i_new_group < p_sys->s_allocated.i_pieces_nbr ; i_new_group ++ )
                if ( p_sys->pi_group_qty[i_new_group] == 0 )
                    break;
            p_sys->ps_pieces[i].i_group_ID = i_new_group;
            p_sys->ps_pieces[i].b_finished = false;
            /* random rotate & mirror */
            switch ( p_sys->s_current_param.i_rotate )
            {
              case 1:
                    puzzle_rotate_pce( p_filter, i, (( (unsigned) vlc_mrand48()) % ( 2 ) ) * 2, p_sys->ps_pieces[i].i_center_x, p_sys->ps_pieces[i].i_center_y, false );
                break;
              case 2:
                    puzzle_rotate_pce( p_filter, i, (( (unsigned) vlc_mrand48()) % ( 4 ) ), p_sys->ps_pieces[i].i_center_x, p_sys->ps_pieces[i].i_center_y, false );
                break;
              case 3:
                    puzzle_rotate_pce( p_filter, i, (( (unsigned) vlc_mrand48()) % ( 8 ) ), p_sys->ps_pieces[i].i_center_x, p_sys->ps_pieces[i].i_center_y, false );
                break;
            }
            /* random mvt */
            p_sys->ps_pieces[i].ps_piece_in_plane[0].i_actual_x =
                    p_sys->ps_desk_planes[0].i_border_width
                    + ( (unsigned) vlc_mrand48()) % ( p_sys->ps_desk_planes[0].i_width - 2*p_sys->ps_desk_planes[0].i_border_width - p_sys->ps_pieces[i].ps_piece_in_plane[0].i_width)
                    + p_sys->ps_pieces[i].ps_piece_in_plane[0].i_width / 2 * ( 1 - p_sys->ps_pieces[i].i_step_x_x )
                    - (p_sys->ps_pieces[i].ps_piece_in_plane[0].i_lines / 2) * p_sys->ps_pieces[i].i_step_y_x;
            p_sys->ps_pieces[i].ps_piece_in_plane[0].i_actual_y =
                    p_sys->ps_desk_planes[0].i_border_lines
                    + ( (unsigned) vlc_mrand48()) % ( p_sys->ps_desk_planes[0].i_lines - 2*p_sys->ps_desk_planes[0].i_border_lines - p_sys->ps_pieces[i].ps_piece_in_plane[0].i_lines)
                    + p_sys->ps_pieces[i].ps_piece_in_plane[0].i_lines / 2 * ( 1 - p_sys->ps_pieces[i].i_step_y_y )
                    - (p_sys->ps_pieces[i].ps_piece_in_plane[0].i_width / 2) * p_sys->ps_pieces[i].i_step_x_y;
            /* redefine shapes */
            uint32_t i_left_pce  = 0;
            uint32_t i_right_pce = 6;
            uint32_t i_top_pce   = 2;
            uint32_t i_btm_pce   = 4;
            uint32_t i_pce = 0;
            for (int32_t i_row = 0; i_row < p_sys->s_allocated.i_rows; i_row++)
                for (int32_t i_col = 0; i_col < p_sys->s_allocated.i_cols; i_col++) {
                    if (p_sys->ps_pieces[i].i_original_row == p_sys->ps_pieces[i_pce].i_original_row) {
                        if (p_sys->ps_pieces[i].i_original_col == p_sys->ps_pieces[i_pce].i_original_col - 1)
                            i_right_pce = i_pce;
                        else if (p_sys->ps_pieces[i].i_original_col == p_sys->ps_pieces[i_pce].i_original_col + 1)
                            i_left_pce = i_pce;
                    }
                    else if (p_sys->ps_pieces[i].i_original_col == p_sys->ps_pieces[i_pce].i_original_col) {
                        if (p_sys->ps_pieces[i].i_original_row == p_sys->ps_pieces[i_pce].i_original_row - 1)
                            i_btm_pce = i_pce;
                        else if (p_sys->ps_pieces[i].i_original_row == p_sys->ps_pieces[i_pce].i_original_row + 1)
                            i_top_pce = i_pce;
                    }
                    i_pce++;
                }
            if ((p_sys->ps_pieces[i].i_left_shape == 0) && (p_sys->ps_pieces[i].i_original_col != 0)) {
                p_sys->ps_pieces[i_left_pce].i_right_shape = 6 + 8 + 8*(( (unsigned) vlc_mrand48()) % ( SHAPES_QTY ) ) + (vlc_mrand48() & 0x01);
                p_sys->ps_pieces[i].i_left_shape = (p_sys->ps_pieces[i_left_pce].i_right_shape - 6 ) ^ 0x01;
            }
            if ((p_sys->ps_pieces[i].i_right_shape == 6) && (p_sys->ps_pieces[i].i_original_col != p_sys->s_allocated.i_cols-1)) {
                p_sys->ps_pieces[i].i_right_shape = 6 + 8 + 8*(( (unsigned) vlc_mrand48()) % ( SHAPES_QTY ) ) + (vlc_mrand48() & 0x01);
                p_sys->ps_pieces[i_right_pce].i_left_shape = (p_sys->ps_pieces[i].i_right_shape - 6 ) ^ 0x01;
            }
            if ((p_sys->ps_pieces[i].i_top_shape == 2) && (p_sys->ps_pieces[i].i_original_row != 0)) {
                p_sys->ps_pieces[i_top_pce].i_btm_shape = 4 + 8 + 8*(( (unsigned) vlc_mrand48()) % ( SHAPES_QTY ) ) + (vlc_mrand48() & 0x01);
                p_sys->ps_pieces[i].i_top_shape = (p_sys->ps_pieces[i_top_pce].i_btm_shape - 2 ) ^ 0x01;
            }
            if ((p_sys->ps_pieces[i].i_btm_shape == 4) && (p_sys->ps_pieces[i].i_original_row != p_sys->s_allocated.i_rows-1)) {
                p_sys->ps_pieces[i].i_btm_shape = 4 + 8 + 8*(( (unsigned) vlc_mrand48()) % ( SHAPES_QTY ) ) + (vlc_mrand48() & 0x01);
                p_sys->ps_pieces[i_btm_pce].i_top_shape = (p_sys->ps_pieces[i].i_btm_shape - 2 ) ^ 0x01;
            }
            puzzle_calculate_corners( p_filter, i );
            break;
        }
    }
}
