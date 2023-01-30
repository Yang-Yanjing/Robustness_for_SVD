}
int puzzle_shuffle( filter_t *p_filter )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    int32_t i_pieces_nbr = p_sys->s_allocated.i_pieces_nbr;
    do
    {
        int i_ret = puzzle_generate_rand_pce_list( p_filter, &p_sys->pi_order );
        if (i_ret != VLC_SUCCESS)
            return i_ret;
    } while( puzzle_is_finished( p_sys, p_sys->pi_order ) || !puzzle_is_valid( p_sys, p_sys->pi_order ) );
    if( p_sys->s_current_param.b_blackslot ) {
        for( int32_t i = 0; i < i_pieces_nbr; i++ )
            if( p_sys->pi_order[i] == i_pieces_nbr - 1 ) {
                p_sys->i_selected = i;
                break;
            }
    }
    else {
        p_sys->i_selected = NO_PCE;
    }
    p_sys->b_shuffle_rqst = false;
    p_sys->b_finished = false;
    return VLC_SUCCESS;
}
