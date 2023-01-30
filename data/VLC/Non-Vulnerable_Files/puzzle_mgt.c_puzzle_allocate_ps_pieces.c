}
int puzzle_allocate_ps_pieces( filter_t *p_filter)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    puzzle_free_ps_pieces(p_filter);
    p_sys->s_allocated.i_pieces_nbr = p_sys->s_allocated.i_rows * p_sys->s_allocated.i_cols;
    p_sys->ps_pieces = malloc( sizeof( piece_t) * p_sys->s_allocated.i_pieces_nbr );
    if( !p_sys->ps_pieces )
        return VLC_ENOMEM;
    for (uint32_t p = 0; p < p_sys->s_allocated.i_pieces_nbr; p++) {
        p_sys->ps_pieces[p].ps_piece_in_plane = malloc( sizeof( piece_in_plane_t) * p_sys->s_allocated.i_planes );
        if( !p_sys->ps_pieces[p].ps_piece_in_plane ) {
            for (uint32_t i=0;i<p;i++)
                free(p_sys->ps_pieces[i].ps_piece_in_plane);
            free(p_sys->ps_pieces);
            p_sys->ps_pieces = NULL;
            return VLC_ENOMEM;
        }
    }
    p_sys->ps_pieces_tmp = malloc( sizeof( piece_t) * p_sys->s_allocated.i_pieces_nbr );
    if( !p_sys->ps_pieces_tmp ) {
        for (uint32_t i=0;i<p_sys->s_allocated.i_pieces_nbr;i++)
            free(p_sys->ps_pieces[i].ps_piece_in_plane);
        free(p_sys->ps_pieces);
        p_sys->ps_pieces = NULL;
        return VLC_ENOMEM;
    }
    p_sys->pi_group_qty = malloc( sizeof( int32_t ) * (p_sys->s_allocated.i_pieces_nbr));
    if( !p_sys->pi_group_qty ) {
        for (uint32_t i=0;i<p_sys->s_allocated.i_pieces_nbr;i++)
            free(p_sys->ps_pieces[i].ps_piece_in_plane);
        free(p_sys->ps_pieces);
        p_sys->ps_pieces = NULL;
        free(p_sys->ps_pieces_tmp);
        p_sys->ps_pieces_tmp = NULL;
        return VLC_ENOMEM;
    }
    return VLC_SUCCESS;
}
