 *****************************************************************************/
void puzzle_auto_solve( filter_t *p_filter)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if ( p_sys->s_current_param.i_auto_solve_speed < 500 )
        return;
    if ( --p_sys->i_auto_solve_countdown_val > 0 )
        return;
    /* delay reached, preset next delay and proceed with puzzle_auto_solve */
    p_sys->i_auto_solve_countdown_val = init_countdown(p_sys->s_current_param.i_auto_solve_speed);
    /* random piece to be moved */
    int32_t i_start = ((unsigned)vlc_mrand48()) % p_sys->s_allocated.i_pieces_nbr;
    /* here the computer will help player by placing the piece at the final location */
    for (uint32_t i_l = 0; i_l < p_sys->s_allocated.i_pieces_nbr; i_l++) {
        int32_t i = ( i_l + i_start ) % p_sys->s_allocated.i_pieces_nbr;
        if ( !p_sys->ps_pieces[i].b_finished ) {
            for (uint32_t j = 0; j < p_sys->s_allocated.i_pieces_nbr; j++) {
                if ( p_sys->ps_pieces[j].i_group_ID == p_sys->ps_pieces[i].i_group_ID ) {
                    p_sys->ps_pieces[j].i_actual_angle   = 0;
                    p_sys->ps_pieces[j].i_actual_mirror  = +1;
                    p_sys->ps_pieces[j].ps_piece_in_plane[0].i_actual_x = p_sys->ps_pieces[j].ps_piece_in_plane[0].i_original_x;
                    p_sys->ps_pieces[j].ps_piece_in_plane[0].i_actual_y = p_sys->ps_pieces[j].ps_piece_in_plane[0].i_original_y;
                    puzzle_calculate_corners( p_filter, j );
                }
            }
            break;
        }
    }
}
