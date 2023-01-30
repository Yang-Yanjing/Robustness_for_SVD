}
bool puzzle_is_valid( filter_sys_t *p_sys, int32_t *pi_pce_lst )
{
    const int32_t i_count = p_sys->s_allocated.i_pieces_nbr;
    if( !p_sys->s_current_param.b_blackslot )
        return true;
    int32_t d = 0;
    for( int32_t i = 0; i < i_count; i++ ) {
        if( pi_pce_lst[i] == i_count - 1 ) {
            d += i / p_sys->s_allocated.i_cols + 1;
            continue;
        }
        for( int32_t j = i+1; j < i_count; j++ ) {
            if( pi_pce_lst[j] == i_count - 1 )
                continue;
            if( pi_pce_lst[i] > pi_pce_lst[j] )
                d++;
        }
    }
    return (d%2) == 0;
}
