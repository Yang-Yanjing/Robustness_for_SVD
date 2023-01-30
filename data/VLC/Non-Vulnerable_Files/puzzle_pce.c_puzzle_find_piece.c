 *****************************************************************************/
int puzzle_find_piece( filter_t *p_filter, int32_t i_x, int32_t i_y, int32_t i_except) {
    filter_sys_t *p_sys = p_filter->p_sys;
    for (uint32_t i = 0; i < p_sys->s_allocated.i_pieces_nbr; i++) {
        piece_t *ps_current_piece = &p_sys->ps_pieces[i];
        if (( ps_current_piece->i_min_x <= i_x ) &&
            ( ps_current_piece->i_max_x >= i_x ) &&
            ( ps_current_piece->i_min_y <= i_y ) &&
            ( ps_current_piece->i_max_y  >= i_y ) &&
            ( (int32_t)i != i_except ) )
        {
            return i;
        }
    }
    return -1;
}
