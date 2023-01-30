 *****************************************************************************/
save_game_t* puzzle_save(filter_t *p_filter)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    save_game_t *ps_save_game = calloc(1, sizeof(*ps_save_game));
    if (!ps_save_game)
        return NULL;
    ps_save_game->i_cols   = p_sys->s_allocated.i_cols;
    ps_save_game->i_rows   = p_sys->s_allocated.i_rows;
    ps_save_game->i_rotate = p_sys->s_allocated.i_rotate;
    ps_save_game->ps_pieces = calloc( ps_save_game->i_cols * ps_save_game->i_rows , sizeof(*ps_save_game->ps_pieces));
    if (!ps_save_game->ps_pieces) {
        free(ps_save_game);
        return NULL;
    }
    int32_t i_border_width = p_sys->ps_desk_planes[0].i_border_width;
    int32_t i_border_lines = p_sys->ps_desk_planes[0].i_border_lines;
    for (int32_t i_pce = 0; i_pce < ps_save_game->i_cols * ps_save_game->i_rows; i_pce++) {
        ps_save_game->ps_pieces[i_pce].i_original_row  = p_sys->ps_pieces[i_pce].i_original_row;
        ps_save_game->ps_pieces[i_pce].i_original_col  = p_sys->ps_pieces[i_pce].i_original_col;
        ps_save_game->ps_pieces[i_pce].i_top_shape     = p_sys->ps_pieces[i_pce].i_top_shape;
        ps_save_game->ps_pieces[i_pce].i_btm_shape     = p_sys->ps_pieces[i_pce].i_btm_shape;
        ps_save_game->ps_pieces[i_pce].i_right_shape   = p_sys->ps_pieces[i_pce].i_right_shape;
        ps_save_game->ps_pieces[i_pce].i_left_shape    = p_sys->ps_pieces[i_pce].i_left_shape;
        ps_save_game->ps_pieces[i_pce].f_pos_x         =(p_sys->ps_pieces[i_pce].ps_piece_in_plane[0].i_actual_x - i_border_width ) / ((float)p_sys->ps_desk_planes[0].i_width - 2*i_border_width);
        ps_save_game->ps_pieces[i_pce].f_pos_y         =(p_sys->ps_pieces[i_pce].ps_piece_in_plane[0].i_actual_y - i_border_lines ) / ((float)p_sys->ps_desk_planes[0].i_lines - 2*i_border_lines);
        ps_save_game->ps_pieces[i_pce].i_actual_angle  = p_sys->ps_pieces[i_pce].i_actual_angle;
        ps_save_game->ps_pieces[i_pce].i_actual_mirror = p_sys->ps_pieces[i_pce].i_actual_mirror;
    }
    return ps_save_game;
}
