/* mouse callback */
int puzzle_mouse( filter_t *p_filter, vlc_mouse_t *p_mouse,
                  const vlc_mouse_t *p_old, const vlc_mouse_t *p_new )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    const video_format_t  *p_fmt_in = &p_filter->fmt_in.video;
    /* Only take events inside the puzzle area */
    if( p_new->i_x < 0 || p_new->i_x >= (int)p_fmt_in->i_width ||
        p_new->i_y < 0 || p_new->i_y >= (int)p_fmt_in->i_height )
        return VLC_EGENERIC;
    if (! p_sys->b_init || p_sys->b_change_param) {
        *p_mouse = *p_new;
        return VLC_SUCCESS;
    }
    p_sys->i_mouse_x = p_new->i_x;
    p_sys->i_mouse_y = p_new->i_y;
    /* If the puzzle is finished, shuffle it if needed */
    if( p_sys->b_finished ) {
        p_sys->b_mouse_drag = false;
        p_sys->b_mouse_mvt = false;
        if( vlc_mouse_HasPressed( p_old, p_new, MOUSE_BUTTON_LEFT ) &&
            p_new->i_x < SHUFFLE_WIDTH && p_new->i_y < SHUFFLE_LINES )
        {
            p_sys->b_shuffle_rqst = true;
            return VLC_EGENERIC;
        }
        else
        {
            /* otherwise we can forward the mouse */
            *p_mouse = *p_new;
            return VLC_SUCCESS;
        }
    }
    if ( !p_sys->s_current_param.b_advanced ) {
        /* "square" game mode (sliding puzzle, swap...) */
        const bool b_clicked = vlc_mouse_HasPressed( p_old, p_new, MOUSE_BUTTON_LEFT );
        if( b_clicked )
        {
            /* */
            const int32_t i_border_width = p_fmt_in->i_width * p_sys->s_current_param.i_border / 100 / 2;
            const int32_t i_border_height = p_fmt_in->i_height * p_sys->s_current_param.i_border / 100 / 2;
            const int32_t i_pos_x = (p_new->i_x - i_border_width) * p_sys->s_allocated.i_cols / (p_fmt_in->i_width - 2*i_border_width);
            const int32_t i_pos_y = (p_new->i_y - i_border_height) * p_sys->s_allocated.i_rows / (p_fmt_in->i_height - 2*i_border_height);
            const int32_t i_pos = i_pos_y * p_sys->s_allocated.i_cols + i_pos_x;
            p_sys->i_mouse_drag_pce = i_pos;
            /* do not take into account if border clicked */
            if ((p_new->i_x <= i_border_width) || (p_new->i_y <=  i_border_height) || (p_new->i_x >= (int) p_fmt_in->i_width -  i_border_width) || (p_new->i_y >= (int) p_fmt_in->i_height -  i_border_height ) )
            {
                *p_mouse = *p_new;
                return VLC_SUCCESS;
            }
            else if( p_sys->i_selected == NO_PCE )
                p_sys->i_selected = i_pos;
            else if( p_sys->i_selected == i_pos && !p_sys->s_current_param.b_blackslot )
                p_sys->i_selected = -1;
            else if( ( p_sys->i_selected == i_pos + 1 && p_sys->i_selected%p_sys->s_allocated.i_cols != 0 )
                  || ( p_sys->i_selected == i_pos - 1 && i_pos % p_sys->s_allocated.i_cols != 0 )
                  || p_sys->i_selected == i_pos +  p_sys->s_allocated.i_cols
                  || p_sys->i_selected == i_pos -  p_sys->s_allocated.i_cols
                  || !p_sys->s_current_param.b_near )
            {
                /* Swap two pieces */
                int32_t a = p_sys->pi_order[ p_sys->i_selected ];
                p_sys->pi_order[ p_sys->i_selected ] = p_sys->pi_order[ i_pos ];
                p_sys->pi_order[ i_pos ] = a;
                /* regen piece location from updated pi_order */
                if ( p_sys->ps_pieces != NULL && p_sys->pi_order != NULL )
                {
                    int32_t i = 0;
                    for (int32_t row = 0; row < p_sys->s_allocated.i_rows; row++) {
                        for (int32_t col = 0; col < p_sys->s_allocated.i_cols; col++) {
                            int32_t orow = p_sys->pi_order[i] / (p_sys->s_allocated.i_cols);
                            int32_t ocol = p_sys->pi_order[i] % (p_sys->s_allocated.i_cols);
                            p_sys->ps_pieces[i].i_original_row = orow;
                            p_sys->ps_pieces[i].i_original_col = ocol;
                            p_sys->ps_pieces[i].i_top_shape    = 0;
                            p_sys->ps_pieces[i].i_btm_shape    = 0;
                            p_sys->ps_pieces[i].i_right_shape  = 0;
                            p_sys->ps_pieces[i].i_left_shape   = 0;
                            p_sys->ps_pieces[i].i_actual_angle = 0;
                            p_sys->ps_pieces[i].i_actual_mirror = +1;
                            p_sys->ps_pieces[i].b_overlap      = false;
                            p_sys->ps_pieces[i].b_finished     = false;
                            p_sys->ps_pieces[i].i_group_ID     = i;
                            for (uint8_t i_plane = 0; i_plane < p_sys->s_allocated.i_planes; i_plane++) {
                                p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_width     = p_sys->ps_puzzle_array[row][col][i_plane].i_width;
                                p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_lines     = p_sys->ps_puzzle_array[row][col][i_plane].i_lines;
                                p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_original_x = p_sys->ps_puzzle_array[orow][ocol][i_plane].i_x;
                                p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_original_y = p_sys->ps_puzzle_array[orow][ocol][i_plane].i_y;
                                p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_actual_x   = p_sys->ps_puzzle_array[row][col][i_plane].i_x;
                                p_sys->ps_pieces[i].ps_piece_in_plane[i_plane].i_actual_y   = p_sys->ps_puzzle_array[row][col][i_plane].i_y;
                            }
                            i++;
                        }
                    }
                }
                p_sys->i_selected = p_sys->s_current_param.b_blackslot ? i_pos : NO_PCE;
                p_sys->b_finished = puzzle_is_finished( p_sys, p_sys->pi_order );
            }
        }
    }
    else /* jigsaw puzzle mode */
    {
        if ((p_sys->ps_desk_planes == NULL)  || (p_sys->ps_pict_planes == NULL)  || (p_sys->ps_puzzle_array == NULL) || (p_sys->ps_pieces == NULL)) {
            *p_mouse = *p_new;
            return VLC_SUCCESS;
        }
        if( vlc_mouse_HasPressed( p_old, p_new, MOUSE_BUTTON_LEFT ) )
        {
            vlc_mutex_lock( &p_sys->pce_lock );
            if (p_sys->i_mouse_drag_pce != NO_PCE) {
                int i_ret = puzzle_piece_foreground( p_filter, p_sys->i_mouse_drag_pce);
                if (i_ret != VLC_SUCCESS)
                {
                    vlc_mutex_unlock( &p_sys->pce_lock );
                    return i_ret;
                }
                p_sys->i_mouse_drag_pce = 0;
                uint32_t i_group_ID = p_sys->ps_pieces[0].i_group_ID;
                for (uint32_t i = 0; i < p_sys->s_allocated.i_pieces_nbr; i++) {
                    if ( i_group_ID == p_sys->ps_pieces[i].i_group_ID ) {
                        p_sys->ps_pieces[i].b_finished = false;
                    }
                    else {
                        break;
                    }
                }
                p_sys->b_mouse_drag = true;
                p_sys->b_mouse_mvt = false;
            }
            else {
            /* player click an empty area then search a piece which is overlapping another one and place it here */
                p_sys->b_mouse_drag = false;
                for (uint32_t i = 0; i < p_sys->s_allocated.i_pieces_nbr; i++)
                    if ( p_sys->ps_pieces[i].b_overlap ) {
                        puzzle_move_group( p_filter, i, p_new->i_x - p_sys->ps_pieces[i].i_center_x,  p_new->i_y - p_sys->ps_pieces[i].i_center_y );
                        p_sys->ps_pieces[i].b_overlap = false;
                        break;
                    }
                p_sys->b_mouse_drag = false;
            }
            vlc_mutex_unlock( &p_sys->pce_lock );
        }
        else if( vlc_mouse_HasReleased( p_old, p_new, MOUSE_BUTTON_LEFT ) )
        {
            if ( !p_sys->b_mouse_mvt && p_sys->b_mouse_drag ) {
                /* piece clicked without any mouse mvt => rotate it or mirror */
                if ( p_sys->s_current_param.i_rotate != 0) {
                    vlc_mutex_lock( &p_sys->pce_lock );
                    uint32_t i_group_ID = p_sys->ps_pieces[0].i_group_ID;
                    for (uint32_t i = 0; i < p_sys->s_allocated.i_pieces_nbr; i++)
                        if ( i_group_ID == p_sys->ps_pieces[i].i_group_ID )
                            puzzle_rotate_pce( p_filter, i, p_sys->i_mouse_action, p_sys->ps_pieces[0].i_center_x, p_sys->ps_pieces[0].i_center_y, p_sys->i_mouse_action != 4 ? true : false );
                    vlc_mutex_unlock( &p_sys->pce_lock );
                }
            }
            p_sys->b_mouse_drag = false;
            p_sys->b_mouse_mvt = false;
        }
        else /* no action on left button */
        {
            /* check if the mouse is in the preview area */
            switch ( p_sys->i_preview_pos )
            {
              case 0:
                if ( p_new->i_x < (int)p_fmt_in->i_width / 2 && p_new->i_y < (int)p_fmt_in->i_height / 2 )
                    p_sys->i_preview_pos++;
                break;
              case 1:
                if ( p_new->i_x > (int)p_fmt_in->i_width / 2 && p_new->i_y < (int)p_fmt_in->i_height / 2 )
                    p_sys->i_preview_pos++;
                break;
              case 2:
                if ( p_new->i_x > (int)p_fmt_in->i_width / 2 && p_new->i_y > (int)p_fmt_in->i_height / 2 )
                    p_sys->i_preview_pos++;
                break;
              case 3:
                if ( p_new->i_x < (int)p_fmt_in->i_width / 2 && p_new->i_y > (int)p_fmt_in->i_height / 2 )
                    p_sys->i_preview_pos++;
                break;
            }
            p_sys->i_preview_pos %= 4;
            if ( !vlc_mouse_IsLeftPressed( p_new ) )
                p_sys->b_mouse_drag = false;
            int i_dx, i_dy;
            vlc_mouse_GetMotion( &i_dx, &i_dy, p_old, p_new );
            if ( i_dx != 0 || i_dy != 0 )
                p_sys->b_mouse_mvt = true;
            if (p_sys->b_mouse_drag) {
                if ( ( p_new->i_x <= 0 ) || ( p_new->i_y <=  0 ) || ( p_new->i_x >= (int) p_fmt_in->i_width )
                        || ( p_new->i_y >= (int) p_fmt_in->i_height ) )
                {
                    /* if the mouse is outside the window, stop moving the piece/group */
                    p_sys->b_mouse_drag = false;
                    p_sys->b_mouse_mvt = true;
                }
                else if ( i_dx != 0 || i_dy != 0 )
                {
                    vlc_mutex_lock( &p_sys->pce_lock );
                    puzzle_move_group( p_filter, p_sys->i_mouse_drag_pce, i_dx, i_dy);
                    vlc_mutex_unlock( &p_sys->pce_lock );
                }
            }
        }
    }
    return VLC_EGENERIC;
}
