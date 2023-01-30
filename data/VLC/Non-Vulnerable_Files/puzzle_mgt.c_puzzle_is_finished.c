}
bool puzzle_is_finished( filter_sys_t *p_sys, int32_t *pi_pce_lst )
{
    for( uint32_t i = 0; i < p_sys->s_allocated.i_pieces_nbr; i++ )
        if( (int32_t)i != pi_pce_lst[i] )
            return false;
    return true;
}
