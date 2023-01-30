 *****************************************************************************/
void puzzle_solve_pces_accuracy( filter_t *p_filter) {
    filter_sys_t *p_sys = p_filter->p_sys;
    p_sys->i_solve_acc_loop++;
    if (p_sys->i_solve_acc_loop >= p_sys->s_allocated.i_pieces_nbr) {
        p_sys->i_done_count = p_sys->i_tmp_done_count;
        p_sys->i_tmp_done_count = 0;
        p_sys->i_solve_acc_loop = 0;
        p_sys->b_finished = (p_sys->i_done_count == p_sys->s_allocated.i_pieces_nbr);
    }
    piece_t *ps_piece = &p_sys->ps_pieces[p_sys->i_solve_acc_loop];
    ps_piece->b_finished = false;
    if (    ps_piece->i_actual_mirror == 1
         && abs( ps_piece->i_TRx - ps_piece->i_ORx )  < p_sys->i_magnet_accuracy
         && abs( ps_piece->i_TRy - ps_piece->i_OTy )  < p_sys->i_magnet_accuracy
         && abs( ps_piece->i_TLx - ps_piece->i_OLx )  < p_sys->i_magnet_accuracy
         && abs( ps_piece->i_TLy - ps_piece->i_OTy )  < p_sys->i_magnet_accuracy )
    {
        uint32_t i_group_ID = ps_piece->i_group_ID;
        p_sys->i_tmp_done_count++;
        for ( uint32_t i = 0; i < p_sys->s_allocated.i_pieces_nbr; i++) {
            ps_piece = &p_sys->ps_pieces[i];
            if ( ( ps_piece->i_group_ID == i_group_ID ) && ( !ps_piece->b_finished ) ) {
                ps_piece->ps_piece_in_plane[0].i_actual_x = ps_piece->i_OLx;
                ps_piece->ps_piece_in_plane[0].i_actual_y = ps_piece->i_OTy;
                ps_piece->i_actual_mirror = +1;
                puzzle_calculate_corners( p_filter, i );
                ps_piece->b_finished = true;
            }
        }
    }
}
