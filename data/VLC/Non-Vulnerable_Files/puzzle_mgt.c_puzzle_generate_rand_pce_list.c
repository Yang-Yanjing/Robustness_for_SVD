}
int puzzle_generate_rand_pce_list( filter_t *p_filter, int32_t **pi_pce_lst )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    int32_t i_pieces_nbr = p_sys->s_allocated.i_pieces_nbr;
    free( *pi_pce_lst );
    *pi_pce_lst = calloc( i_pieces_nbr, sizeof(**pi_pce_lst) );
    if( !*pi_pce_lst )
        return VLC_ENOMEM;
    for( int32_t i = 0; i < i_pieces_nbr; i++ )
        (*pi_pce_lst)[i] = NO_PCE;
    for( int32_t c = 0; c < i_pieces_nbr; ) {
        int32_t i = ((unsigned)vlc_mrand48()) % i_pieces_nbr;
        if( (*pi_pce_lst)[i] == NO_PCE )
            (*pi_pce_lst)[i] = c++;
    }
    return VLC_SUCCESS;
}
