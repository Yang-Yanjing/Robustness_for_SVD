}
int puzzle_piece_foreground( filter_t *p_filter, int32_t i_piece) {
    filter_sys_t *p_sys = p_filter->p_sys;
    piece_t *ps_pieces_tmp;        /* list [piece] of pieces data. Sort as per layers */
    uint32_t i_group_ID = p_sys->ps_pieces[i_piece].i_group_ID;
    ps_pieces_tmp = malloc( sizeof( piece_t) * p_sys->s_allocated.i_pieces_nbr );
    if (!ps_pieces_tmp)
        return VLC_ENOMEM;
    int32_t j=0;
    memcpy( &ps_pieces_tmp[j], &p_sys->ps_pieces[i_piece], sizeof(piece_t) );
    j++;
    for (uint32_t i = 0; i < p_sys->s_allocated.i_pieces_nbr; i++) {
        if ( ( p_sys->ps_pieces[i].i_group_ID == i_group_ID ) && ( (int32_t)i != i_piece ) ) {
            memcpy( &ps_pieces_tmp[j], &p_sys->ps_pieces[i], sizeof(piece_t));
            j++;
        }
    }
    for (uint32_t i = 0; i < p_sys->s_allocated.i_pieces_nbr; i++) {
        if ( p_sys->ps_pieces[i].i_group_ID != i_group_ID ) {
            memcpy( &ps_pieces_tmp[j], &p_sys->ps_pieces[i], sizeof(piece_t));
            j++;
        }
    }
    free( p_filter->p_sys->ps_pieces );
    p_filter->p_sys->ps_pieces = ps_pieces_tmp;
    return VLC_SUCCESS;
}
