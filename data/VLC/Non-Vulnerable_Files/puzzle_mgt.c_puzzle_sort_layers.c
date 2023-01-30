 *****************************************************************************/
int puzzle_sort_layers( filter_t *p_filter)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    int32_t i_idx = 0;
    for (uint32_t i_qty = 1; i_qty <= p_sys->s_current_param.i_pieces_nbr; i_qty++) {
        /* pieces at the wrong place are in foreground */
        for (uint32_t i_pce_loop = 0; i_pce_loop < p_sys->s_current_param.i_pieces_nbr; i_pce_loop++) {
            uint32_t i_grp = p_sys->ps_pieces[i_pce_loop].i_group_ID;
            if ( p_sys->pi_group_qty[i_grp] == (int32_t)i_qty ) {
                bool b_check_ok = true;
                for (int32_t i_pce_check = 0; i_pce_check < i_idx; i_pce_check++)
                    if ( p_sys->ps_pieces_tmp[i_pce_check].i_group_ID == i_grp )
                        b_check_ok = false;
                if ( b_check_ok )
                    for (uint32_t i_pce = i_pce_loop; i_pce < p_sys->s_current_param.i_pieces_nbr; i_pce++)
                        if (( p_sys->ps_pieces[i_pce].i_group_ID == i_grp ) && !p_sys->ps_pieces[i_pce].b_finished ) {
                            memcpy( &p_sys->ps_pieces_tmp[i_idx], &p_sys->ps_pieces[i_pce], sizeof(piece_t));
                            i_idx++;
                        }
            }
        }
        /* pieces at the final location are in background */
        for (uint32_t i_pce_loop = 0; i_pce_loop < p_sys->s_current_param.i_pieces_nbr; i_pce_loop++) {
            uint32_t i_grp = p_sys->ps_pieces[i_pce_loop].i_group_ID;
            if ( p_sys->pi_group_qty[i_grp] == (int32_t)i_qty ) {
                bool b_check_ok = true;
                for (int32_t i_pce_check = 0; i_pce_check < i_idx; i_pce_check++)
                    if ( p_sys->ps_pieces_tmp[i_pce_check].i_group_ID == i_grp && p_sys->ps_pieces_tmp[i_pce_check].b_finished )
                        b_check_ok = false;
                if ( b_check_ok )
                    for (uint32_t i_pce = i_pce_loop; i_pce < p_sys->s_current_param.i_pieces_nbr; i_pce++)
                        if (( p_sys->ps_pieces[i_pce].i_group_ID == i_grp ) && p_sys->ps_pieces[i_pce].b_finished ) {
                            memcpy( &p_sys->ps_pieces_tmp[i_idx], &p_sys->ps_pieces[i_pce], sizeof(piece_t));
                            i_idx++;
                        }
            }
        }
    }
    free( p_filter->p_sys->ps_pieces );
    p_filter->p_sys->ps_pieces = p_sys->ps_pieces_tmp;
    p_sys->ps_pieces_tmp = malloc( sizeof( piece_t) * p_sys->s_allocated.i_pieces_nbr );
    if (!p_sys->ps_pieces_tmp)
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
