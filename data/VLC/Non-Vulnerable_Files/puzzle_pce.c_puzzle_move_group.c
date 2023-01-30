 *****************************************************************************/
void puzzle_move_group( filter_t *p_filter, int32_t i_piece, int32_t i_dx, int32_t i_dy)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    uint32_t i_group_ID = p_sys->ps_pieces[i_piece].i_group_ID;
    for (uint32_t i = 0; i < p_sys->s_allocated.i_pieces_nbr; i++) {
        piece_t *ps_piece = &p_sys->ps_pieces[i];
        if (ps_piece->i_group_ID == i_group_ID) {
            ps_piece->b_finished = false;
            ps_piece->ps_piece_in_plane[0].i_actual_x += i_dx;
            ps_piece->ps_piece_in_plane[0].i_actual_y += i_dy;
            puzzle_calculate_corners( p_filter, i );
        }
    }
}
