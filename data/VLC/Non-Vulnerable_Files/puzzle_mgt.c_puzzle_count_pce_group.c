}
void puzzle_count_pce_group( filter_t *p_filter) { /* count pce in each group */
    filter_sys_t *p_sys = p_filter->p_sys;
    memset ( p_sys->pi_group_qty, 0, sizeof( int32_t ) * (p_sys->s_allocated.i_pieces_nbr) );
    for (uint32_t i_pce = 0; i_pce < p_sys->s_allocated.i_pieces_nbr; i_pce++)
        p_sys->pi_group_qty[p_sys->ps_pieces[i_pce].i_group_ID]++;
}
