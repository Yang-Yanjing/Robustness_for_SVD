 *****************************************************************************/
int puzzle_bake( filter_t *p_filter, picture_t *p_pic_out, picture_t *p_pic_in)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    int i_ret = 0;
    puzzle_free_ps_puzzle_array ( p_filter );
    puzzle_free_ps_pieces_shapes ( p_filter);
    puzzle_free_ps_pieces ( p_filter );
    p_sys->s_allocated.i_rows =          p_sys->s_current_param.i_rows;
    p_sys->s_allocated.i_cols =          p_sys->s_current_param.i_cols;
    p_sys->s_allocated.i_planes =        p_sys->s_current_param.i_planes;
    p_sys->s_allocated.i_piece_types =   ((p_sys->s_current_param.b_advanced)?PIECE_TYPE_NBR:0);
    p_sys->s_allocated.i_pieces_nbr =    p_sys->s_allocated.i_rows * p_sys->s_allocated.i_cols;
    p_sys->s_allocated.b_preview =       p_sys->s_current_param.b_preview;
    p_sys->s_allocated.i_preview_size =  p_sys->s_current_param.i_preview_size;
    p_sys->s_allocated.i_border =        p_sys->s_current_param.i_border;
    p_sys->s_allocated.b_blackslot =     p_sys->s_current_param.b_blackslot;
    p_sys->s_allocated.b_near =          p_sys->s_current_param.b_near;
    p_sys->s_allocated.i_shape_size =    p_sys->s_current_param.i_shape_size;
    p_sys->s_allocated.b_advanced =      p_sys->s_current_param.b_advanced;
    p_sys->s_allocated.i_auto_shuffle_speed = p_sys->s_current_param.i_auto_shuffle_speed;
    p_sys->s_allocated.i_auto_solve_speed =   p_sys->s_current_param.i_auto_solve_speed;
    p_sys->s_allocated.i_rotate =        p_sys->s_current_param.i_rotate;
    p_sys->ps_puzzle_array = malloc( sizeof( puzzle_array_t** ) * (p_sys->s_allocated.i_rows + 1));
    if( !p_sys->ps_puzzle_array )
        return VLC_ENOMEM;
    for (int32_t r=0; r < p_sys->s_allocated.i_rows + 1; r++) {
        p_sys->ps_puzzle_array[r] = malloc( sizeof( puzzle_array_t* ) * (p_sys->s_allocated.i_cols + 1));
        if( !p_sys->ps_puzzle_array[r] )
            return VLC_ENOMEM;
        for (int32_t c=0; c < p_sys->s_allocated.i_cols + 1; c++) {
            p_sys->ps_puzzle_array[r][c] = malloc( sizeof( puzzle_array_t ) * p_sys->s_allocated.i_planes);
            if( !p_sys->ps_puzzle_array[r][c] )
                return VLC_ENOMEM;
        }
    }
    p_sys->ps_desk_planes = malloc( sizeof( puzzle_plane_t ) * p_sys->s_allocated.i_planes);
    if( !p_sys->ps_desk_planes )
        return VLC_ENOMEM;
    p_sys->ps_pict_planes = malloc( sizeof( puzzle_plane_t ) * p_sys->s_allocated.i_planes);
    if( !p_sys->ps_pict_planes )
        return VLC_ENOMEM;
    for (uint8_t i_plane = 0; i_plane < p_sys->s_allocated.i_planes; i_plane++) {
        p_sys->ps_desk_planes[i_plane].i_lines = p_pic_out->p[i_plane].i_visible_lines;
        p_sys->ps_desk_planes[i_plane].i_pitch = p_pic_out->p[i_plane].i_pitch;
        p_sys->ps_desk_planes[i_plane].i_visible_pitch = p_pic_out->p[i_plane].i_visible_pitch;
        p_sys->ps_desk_planes[i_plane].i_pixel_pitch = p_pic_out->p[i_plane].i_pixel_pitch;
        p_sys->ps_desk_planes[i_plane].i_width = p_pic_out->p[i_plane].i_visible_pitch / p_pic_out->p[i_plane].i_pixel_pitch;
        p_sys->ps_desk_planes[i_plane].i_preview_width = p_sys->ps_desk_planes[i_plane].i_width * p_sys->s_current_param.i_preview_size / 100;
        p_sys->ps_desk_planes[i_plane].i_preview_lines = p_sys->ps_desk_planes[i_plane].i_lines * p_sys->s_current_param.i_preview_size / 100;
        p_sys->ps_desk_planes[i_plane].i_border_width = p_sys->ps_desk_planes[i_plane].i_width * p_sys->s_current_param.i_border / 2 / 100;
        p_sys->ps_desk_planes[i_plane].i_border_lines = p_sys->ps_desk_planes[i_plane].i_lines * p_sys->s_current_param.i_border / 2 / 100;
        p_sys->ps_desk_planes[i_plane].i_pce_max_width = (( p_sys->ps_desk_planes[i_plane].i_width
                - 2 * p_sys->ps_desk_planes[i_plane].i_border_width ) + p_sys->s_allocated.i_cols - 1 ) / p_sys->s_allocated.i_cols;
        p_sys->ps_desk_planes[i_plane].i_pce_max_lines = (( p_sys->ps_desk_planes[i_plane].i_lines
                - 2 * p_sys->ps_desk_planes[i_plane].i_border_lines ) + p_sys->s_allocated.i_rows - 1 ) / p_sys->s_allocated.i_rows;
        p_sys->ps_pict_planes[i_plane].i_lines = p_pic_in->p[i_plane].i_visible_lines;
        p_sys->ps_pict_planes[i_plane].i_pitch = p_pic_in->p[i_plane].i_pitch;
        p_sys->ps_pict_planes[i_plane].i_visible_pitch = p_pic_in->p[i_plane].i_visible_pitch;
        p_sys->ps_pict_planes[i_plane].i_pixel_pitch = p_pic_in->p[i_plane].i_pixel_pitch;
        p_sys->ps_pict_planes[i_plane].i_width = p_pic_in->p[i_plane].i_visible_pitch / p_pic_in->p[i_plane].i_pixel_pitch;
        p_sys->ps_pict_planes[i_plane].i_preview_width = p_sys->ps_pict_planes[i_plane].i_width * p_sys->s_current_param.i_preview_size / 100;
        p_sys->ps_pict_planes[i_plane].i_preview_lines = p_sys->ps_pict_planes[i_plane].i_lines * p_sys->s_current_param.i_preview_size / 100;
        p_sys->ps_pict_planes[i_plane].i_border_width = p_sys->ps_pict_planes[i_plane].i_width * p_sys->s_current_param.i_border / 2 / 100;
        p_sys->ps_pict_planes[i_plane].i_border_lines = p_sys->ps_pict_planes[i_plane].i_lines * p_sys->s_current_param.i_border / 2 / 100;
        p_sys->ps_pict_planes[i_plane].i_pce_max_width = (( p_sys->ps_desk_planes[i_plane].i_width
                - 2 * p_sys->ps_pict_planes[i_plane].i_border_width ) + p_sys->s_allocated.i_cols - 1 ) / p_sys->s_allocated.i_cols;
        p_sys->ps_pict_planes[i_plane].i_pce_max_lines = (( p_sys->ps_pict_planes[i_plane].i_lines
                - 2 * p_sys->ps_pict_planes[i_plane].i_border_lines ) + p_sys->s_allocated.i_rows - 1 ) / p_sys->s_allocated.i_rows;
        for (int32_t r = 0; r < p_sys->s_allocated.i_rows; r++)
            for (int32_t c = 0; c < p_sys->s_allocated.i_cols; c++) {
                if ( r == 0 )
                    p_sys->ps_puzzle_array[r][c][i_plane].i_y = p_sys->ps_pict_planes[i_plane].i_border_lines;
                if ( c == 0 )
                    p_sys->ps_puzzle_array[r][c][i_plane].i_x = p_sys->ps_pict_planes[i_plane].i_border_width;
                p_sys->ps_puzzle_array[r][c][i_plane].i_width =
                    (p_sys->ps_pict_planes[i_plane].i_width - p_sys->ps_pict_planes[i_plane].i_border_width
                    - p_sys->ps_puzzle_array[r][c][i_plane].i_x) / ( p_sys->s_allocated.i_cols - c );
                p_sys->ps_puzzle_array[r][c][i_plane].i_lines =
                    (p_sys->ps_pict_planes[i_plane].i_lines - p_sys->ps_pict_planes[i_plane].i_border_lines
                    - p_sys->ps_puzzle_array[r][c][i_plane].i_y) / ( p_sys->s_allocated.i_rows - r );
                p_sys->ps_puzzle_array[r][c + 1][i_plane].i_x =
                    p_sys->ps_puzzle_array[r][c][i_plane].i_x + p_sys->ps_puzzle_array[r][c][i_plane].i_width;
                p_sys->ps_puzzle_array[r + 1][c][i_plane].i_y =
                    p_sys->ps_puzzle_array[r][c][i_plane].i_y + p_sys->ps_puzzle_array[r][c][i_plane].i_lines;
            }
    }
    p_sys->i_magnet_accuracy = ( p_sys->s_current_param.i_pict_width / 50) + 3;
    if (p_sys->s_current_param.b_advanced && p_sys->s_allocated.i_shape_size != 0) {
        i_ret = puzzle_bake_pieces_shapes ( p_filter );
        if (i_ret != VLC_SUCCESS)
            return i_ret;
    }
    i_ret = puzzle_bake_piece ( p_filter );
    if (i_ret != VLC_SUCCESS)
        return i_ret;
    if ( (p_sys->pi_order != NULL) && (p_sys->ps_desk_planes != NULL) && (p_sys->ps_pict_planes != NULL)
        && (p_sys->ps_puzzle_array != NULL) && (p_sys->ps_pieces != NULL) )
        p_sys->b_init = true;
    if ( (p_sys->ps_pieces_shapes == NULL) && (p_sys->s_current_param.b_advanced)
        && (p_sys->s_current_param.i_shape_size != 0) )
        p_sys->b_init = false;
    return VLC_SUCCESS;
}
