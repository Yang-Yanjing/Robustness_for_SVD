}
void puzzle_free_ps_pieces( filter_t *p_filter)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if (p_sys->ps_pieces != NULL) {
        for (uint32_t i_pce = 0; i_pce < p_sys->s_allocated.i_pieces_nbr; i_pce++)
            free( p_sys->ps_pieces[i_pce].ps_piece_in_plane );
        free( p_sys->ps_pieces );
    }
    p_sys->ps_pieces = NULL;
    free( p_sys->pi_order );
    p_sys->pi_order = NULL;
    free( p_sys->ps_pieces_tmp );
    p_sys->ps_pieces_tmp = NULL;
    free( p_sys->pi_group_qty );
    p_sys->pi_group_qty = NULL;
    return;
}
