
picture_t *Filter( filter_t *p_filter, picture_t *p_pic_in ) {
    if( !p_pic_in || !p_filter) return NULL;
    const video_format_t  *p_fmt_in = &p_filter->fmt_in.video;
    filter_sys_t *p_sys = p_filter->p_sys;
    picture_t *p_pic_out = filter_NewPicture( p_filter );
    if( !p_pic_out ) {
        picture_Release( p_pic_in );
        return NULL;
    }
    int i_ret = 0;
    p_sys->b_bake_request = false;
    if ((p_sys->pi_order == NULL) || (p_sys->ps_desk_planes == NULL) || (p_sys->ps_pict_planes == NULL)  || (p_sys->ps_puzzle_array == NULL) || (p_sys->ps_pieces == NULL))
        p_sys->b_init = false;
    if ((p_sys->ps_pieces_shapes == NULL) && p_sys->s_current_param.b_advanced && (p_sys->s_current_param.i_shape_size != 0))
        p_sys->b_init = false;
    /* assert initialized & allocated data match with current frame characteristics */
    if ( p_sys->s_allocated.i_planes != p_pic_out->i_planes)
        p_sys->b_init = false;
    p_sys->s_current_param.i_planes = p_pic_out->i_planes;
    if (p_sys->ps_pict_planes != NULL) {
        for (uint8_t i_plane = 0; i_plane < p_sys->s_allocated.i_planes; i_plane++) {
            if ( (p_sys->ps_pict_planes[i_plane].i_lines != p_pic_in->p[i_plane].i_visible_lines)
                    || (p_sys->ps_pict_planes[i_plane].i_width != p_pic_in->p[i_plane].i_visible_pitch / p_pic_in->p[i_plane].i_pixel_pitch)
                    || (p_sys->ps_desk_planes[i_plane].i_lines != p_pic_out->p[i_plane].i_visible_lines)
                    || (p_sys->ps_desk_planes[i_plane].i_width != p_pic_out->p[i_plane].i_visible_pitch / p_pic_out->p[i_plane].i_pixel_pitch) )
                p_sys->b_init = false;
        }
    }
    p_sys->s_current_param.i_pict_width  = (int) p_pic_in->p[0].i_visible_pitch / p_pic_in->p[0].i_pixel_pitch;
    p_sys->s_current_param.i_pict_height = (int) p_pic_in->p[0].i_visible_lines;
    p_sys->s_current_param.i_desk_width  = (int) p_pic_out->p[0].i_visible_pitch / p_pic_out->p[0].i_pixel_pitch;
    p_sys->s_current_param.i_desk_height = (int) p_pic_out->p[0].i_visible_lines;
    /* assert no mismatch between sizes */
    if (    p_sys->s_current_param.i_pict_width  != p_sys->s_current_param.i_desk_width
         || p_sys->s_current_param.i_pict_height != p_sys->s_current_param.i_desk_height
         || p_sys->s_current_param.i_pict_width  != (int) p_fmt_in->i_visible_width
         || p_sys->s_current_param.i_pict_height != (int) p_fmt_in->i_visible_height ) {
        picture_Release(p_pic_in);
        picture_Release(p_pic_out);
        return NULL;
    }
    vlc_mutex_lock( &p_sys->lock );
    /* check if we have to compute initial data */
    if ( p_sys->b_change_param || p_sys->b_bake_request || !p_sys->b_init ) {
        if ( p_sys->s_allocated.i_rows != p_sys->s_new_param.i_rows
                || p_sys->s_allocated.i_cols != p_sys->s_new_param.i_cols
                || p_sys->s_allocated.i_rotate != p_sys->s_new_param.i_rotate
                || p_sys->s_allocated.i_mode != p_sys->s_new_param.i_mode
                || p_sys->b_bake_request  || !p_sys->b_init )
        {
            p_sys->b_bake_request = true;
            p_sys->b_init = false;
            p_sys->b_shuffle_rqst = true;
            p_sys->b_shape_init = false;
        }
        if ( p_sys->s_current_param.i_border != p_sys->s_new_param.i_border
                || p_sys->s_current_param.i_shape_size != p_sys->s_new_param.i_shape_size )
        {
            p_sys->b_bake_request = true;
            p_sys->b_shape_init = false;
        }
        /* depending on the game selected, set associated internal flags */
        switch ( p_sys->s_new_param.i_mode )
        {
          case 0:  /* jigsaw puzzle */
            p_sys->s_new_param.b_advanced    = true;
            p_sys->s_new_param.b_blackslot   = false;
            p_sys->s_new_param.b_near        = false;
            break;
          case 1:  /* sliding puzzle */
            p_sys->s_new_param.b_advanced    = false;
            p_sys->s_new_param.b_blackslot   = true;
            p_sys->s_new_param.b_near        = true;
            break;
          case 2:  /* swap puzzle */
            p_sys->s_new_param.b_advanced    = false;
            p_sys->s_new_param.b_blackslot   = false;
            p_sys->s_new_param.b_near        = true;
            break;
          case 3:  /* exchange puzzle */
            p_sys->s_new_param.b_advanced    = false;
            p_sys->s_new_param.b_blackslot   = false;
            p_sys->s_new_param.b_near        = false;
            break;
        }
        p_sys->s_current_param.i_mode = p_sys->s_new_param.i_mode;
        if ( p_sys->s_current_param.b_blackslot != p_sys->s_new_param.b_blackslot
                && p_sys->i_selected == NO_PCE
                && p_sys->s_current_param.b_blackslot )
            p_sys->i_selected = 0;
        if ( p_sys->s_current_param.i_auto_shuffle_speed != p_sys->s_new_param.i_auto_shuffle_speed )
            p_sys->i_auto_shuffle_countdown_val = init_countdown(p_sys->s_new_param.i_auto_shuffle_speed);
        if ( p_sys->s_current_param.i_auto_solve_speed != p_sys->s_new_param.i_auto_solve_speed )
            p_sys->i_auto_solve_countdown_val = init_countdown(p_sys->s_current_param.i_auto_solve_speed);
        p_sys->s_current_param.i_rows       = p_sys->s_new_param.i_rows;
        p_sys->s_current_param.i_cols       = p_sys->s_new_param.i_cols;
        p_sys->s_current_param.i_pieces_nbr = p_sys->s_current_param.i_rows * p_sys->s_current_param.i_cols;
        p_sys->s_current_param.b_advanced   = p_sys->s_new_param.b_advanced;
        if (!p_sys->s_new_param.b_advanced) {
            p_sys->s_current_param.b_blackslot   = p_sys->s_new_param.b_blackslot;
            p_sys->s_current_param.b_near        = p_sys->s_new_param.b_near || p_sys->s_new_param.b_blackslot;
            p_sys->s_current_param.i_border      = 0;
            p_sys->s_current_param.b_preview     = false;
            p_sys->s_current_param.i_preview_size= 0;
            p_sys->s_current_param.i_shape_size  = 0;
            p_sys->s_current_param.i_auto_shuffle_speed  = 0;
            p_sys->s_current_param.i_auto_solve_speed    = 0;
            p_sys->s_current_param.i_rotate      = 0;
        }
        else
        {
            p_sys->s_current_param.b_blackslot = false;
            p_sys->s_current_param.b_near      = false;
            p_sys->s_current_param.i_border    = p_sys->s_new_param.i_border;
            p_sys->s_current_param.b_preview   = p_sys->s_new_param.b_preview;
            p_sys->s_current_param.i_preview_size        = p_sys->s_new_param.i_preview_size;
            p_sys->s_current_param.i_shape_size          = p_sys->s_new_param.i_shape_size;
            p_sys->s_current_param.i_auto_shuffle_speed  = p_sys->s_new_param.i_auto_shuffle_speed;
            p_sys->s_current_param.i_auto_solve_speed    = p_sys->s_new_param.i_auto_solve_speed;
            p_sys->s_current_param.i_rotate     = p_sys->s_new_param.i_rotate;
        }
        p_sys->b_change_param = false;
    }
    vlc_mutex_unlock( &p_sys->lock );
    /* generate initial puzzle data when needed */
    if ( p_sys->b_bake_request ) {
        if (!p_sys->b_shuffle_rqst) {
            /* here we have to keep the same position
             * we have to save locations before generating new data
             */
            save_game_t *ps_save_game = puzzle_save(p_filter);
            if (!ps_save_game)
                return CopyInfoAndRelease( p_pic_out, p_pic_in );
            i_ret = puzzle_bake( p_filter, p_pic_out, p_pic_in );
            if ( i_ret != VLC_SUCCESS )
            {
                free(ps_save_game->ps_pieces);
                free(ps_save_game);
                return CopyInfoAndRelease( p_pic_out, p_pic_in );
            }
            puzzle_load( p_filter, ps_save_game);
            free(ps_save_game->ps_pieces);
            free(ps_save_game);
        }
        else {
            i_ret = puzzle_bake( p_filter, p_pic_out, p_pic_in );
            if ( i_ret != VLC_SUCCESS )
                return CopyInfoAndRelease( p_pic_out, p_pic_in );
        }
    }
    /* shuffle the desk and generate pieces data  */
    if ( p_sys->b_shuffle_rqst && p_sys->b_init ) {
        i_ret = puzzle_bake_piece ( p_filter );
        if (i_ret != VLC_SUCCESS)
            return CopyInfoAndRelease( p_pic_out, p_pic_in );
    }
    /* preset output pic */
    if ( !p_sys->b_bake_request && !p_sys->b_shuffle_rqst && p_sys->b_init && !p_sys->b_finished )
        puzzle_preset_desk_background(p_pic_out, 0, 127, 127);
    else {
        /* copy src to dst during init & bake process */
        for( uint8_t i_plane = 0; i_plane < p_pic_out->i_planes; i_plane++ )
            memcpy( p_pic_out->p[i_plane].p_pixels, p_pic_in->p[i_plane].p_pixels,
                p_pic_in->p[i_plane].i_pitch * (int32_t) p_pic_in->p[i_plane].i_visible_lines );
    }
    vlc_mutex_lock( &p_sys->pce_lock );
    /* manage the game, adjust locations, groups and regenerate some corrupted data if any */
    for (uint32_t i = 0; i < __MAX( 4, p_sys->s_allocated.i_pieces_nbr / 4 )
                             && ( !p_sys->b_bake_request && !p_sys->b_mouse_drag
                             && p_sys->b_init && p_sys->s_current_param.b_advanced ); i++)
    {
        puzzle_solve_pces_accuracy( p_filter );
    }
    for (uint32_t i = 0; i < __MAX( 4, p_sys->s_allocated.i_pieces_nbr / 4 )
                             && ( !p_sys->b_bake_request && !p_sys->b_mouse_drag
                             && p_sys->b_init && p_sys->s_current_param.b_advanced ); i++)
    {
        puzzle_solve_pces_group( p_filter );
    }
    if ( !p_sys->b_bake_request && !p_sys->b_mouse_drag && p_sys->b_init
            && p_sys->s_current_param.b_advanced )
        puzzle_count_pce_group( p_filter);
    if ( !p_sys->b_bake_request && !p_sys->b_mouse_drag && p_sys->b_init
            && p_sys->s_current_param.b_advanced ) {
        i_ret = puzzle_sort_layers( p_filter);
        if (i_ret != VLC_SUCCESS)
        {
            vlc_mutex_unlock( &p_sys->pce_lock );
            return CopyInfoAndRelease( p_pic_out, p_pic_in );
        }
    }
    for (uint32_t i = 0; i < __MAX( 4, p_sys->s_allocated.i_pieces_nbr / 24 )
                            && ( !p_sys->b_bake_request && !p_sys->b_mouse_drag
                            && p_sys->b_init && p_sys->s_current_param.b_advanced ); i++)
    {
        p_sys->i_calc_corn_loop++;
        p_sys->i_calc_corn_loop %= p_sys->s_allocated.i_pieces_nbr;
        puzzle_calculate_corners( p_filter, p_sys->i_calc_corn_loop );
    }
    /* computer moves some piece depending on auto_solve and auto_shuffle param */
    if ( !p_sys->b_bake_request && !p_sys->b_mouse_drag && p_sys->b_init
             && p_sys->ps_puzzle_array != NULL && p_sys->s_current_param.b_advanced )
    {
        puzzle_auto_shuffle( p_filter );
        puzzle_auto_solve( p_filter );
    }
    vlc_mutex_unlock( &p_sys->pce_lock );
    /* draw output pic */
    if ( !p_sys->b_bake_request && p_sys->b_init  && p_sys->ps_puzzle_array != NULL ) {
        puzzle_draw_borders(p_filter, p_pic_in, p_pic_out);
        p_sys->i_pointed_pce = NO_PCE;
        puzzle_draw_pieces(p_filter, p_pic_in, p_pic_out);
        /* when puzzle_draw_pieces() has not updated p_sys->i_pointed_pce,
         * use puzzle_find_piece to define the piece pointed by the mouse
         */
        if (p_sys->i_pointed_pce == NO_PCE)
            p_sys->i_mouse_drag_pce = puzzle_find_piece( p_filter, p_sys->i_mouse_x, p_sys->i_mouse_y, -1);
        else
            p_sys->i_mouse_drag_pce = p_sys->i_pointed_pce;
        if (p_sys->s_current_param.b_preview )
            puzzle_draw_preview(p_filter, p_pic_in, p_pic_out);
        /* highlight the selected piece when not playing jigsaw mode */
        if ( p_sys->i_selected != NO_PCE && !p_sys->s_current_param.b_blackslot
                && !p_sys->s_current_param.b_advanced )
        {
            int32_t c = (p_sys->i_selected % p_sys->s_allocated.i_cols);
            int32_t r = (p_sys->i_selected / p_sys->s_allocated.i_cols);
            puzzle_draw_rectangle(p_pic_out,
                p_sys->ps_puzzle_array[r][c][0].i_x,
                p_sys->ps_puzzle_array[r][c][0].i_y,
                p_sys->ps_puzzle_array[r][c][0].i_width,
                p_sys->ps_puzzle_array[r][c][0].i_lines,
                255, 127, 127);
        }
        /* draw the blackslot when playing sliding puzzle mode */
        if ( p_sys->i_selected != NO_PCE && p_sys->s_current_param.b_blackslot
                && !p_sys->s_current_param.b_advanced )
        {
            int32_t c = (p_sys->i_selected % p_sys->s_allocated.i_cols);
            int32_t r = (p_sys->i_selected / p_sys->s_allocated.i_cols);
            puzzle_fill_rectangle(p_pic_out,
                p_sys->ps_puzzle_array[r][c][0].i_x,
                p_sys->ps_puzzle_array[r][c][0].i_y,
                p_sys->ps_puzzle_array[r][c][0].i_width,
                p_sys->ps_puzzle_array[r][c][0].i_lines,
                0, 127, 127);
        }
        /* Draw the 'puzzle_shuffle' button if the puzzle is finished */
        if ( p_sys->b_finished )
            puzzle_draw_sign(p_pic_out, 0, 0, SHUFFLE_WIDTH, SHUFFLE_LINES, ppsz_shuffle_button, false);
        /* draw an arrow at mouse pointer to indicate current action (rotation...) */
        if ((p_sys->i_mouse_drag_pce != NO_PCE) && !p_sys->b_mouse_drag
                && !p_sys->b_finished && p_sys->s_current_param.b_advanced )
        {
            vlc_mutex_lock( &p_sys->pce_lock );
            int32_t i_delta_x;
            if (p_sys->s_current_param.i_rotate != 3)
                i_delta_x = 0;
            else if ( (p_sys->ps_pieces[p_sys->i_mouse_drag_pce].i_actual_angle & 1) == 0)
                i_delta_x = p_sys->ps_desk_planes[0].i_pce_max_width / 6;
            else
                i_delta_x = p_sys->ps_desk_planes[0].i_pce_max_lines / 6;
            if (p_sys->s_current_param.i_rotate == 0)
                p_sys->i_mouse_action = 0;
            else if (p_sys->s_current_param.i_rotate == 1)
                p_sys->i_mouse_action = 2;
            else if ( p_sys->i_mouse_x >= ( p_sys->ps_pieces[p_sys->i_mouse_drag_pce].i_center_x + i_delta_x) )
                p_sys->i_mouse_action = -1;  /* rotate counterclockwise */
            else if ( p_sys->i_mouse_x <= ( p_sys->ps_pieces[p_sys->i_mouse_drag_pce].i_center_x - i_delta_x) )
                p_sys->i_mouse_action = +1;
            else
                p_sys->i_mouse_action = 4;   /* center click: only mirror */
            if ( p_sys->i_mouse_action == +1 )
                puzzle_draw_sign(p_pic_out, p_sys->i_mouse_x - ARROW_WIDTH,
                                 p_sys->i_mouse_y, ARROW_WIDTH, ARROW_LINES, ppsz_rot_arrow_sign, false);
            else if ( p_sys->i_mouse_action == -1 )
                puzzle_draw_sign(p_pic_out, p_sys->i_mouse_x - ARROW_WIDTH,
                                 p_sys->i_mouse_y, ARROW_WIDTH, ARROW_LINES, ppsz_rot_arrow_sign, true);
            else if ( p_sys->i_mouse_action == 4 )
                puzzle_draw_sign(p_pic_out, p_sys->i_mouse_x - ARROW_WIDTH,
                                 p_sys->i_mouse_y, ARROW_WIDTH, ARROW_LINES, ppsz_mir_arrow_sign, false);
            vlc_mutex_unlock( &p_sys->pce_lock );
        }
    }
    return CopyInfoAndRelease( p_pic_out, p_pic_in );
}
