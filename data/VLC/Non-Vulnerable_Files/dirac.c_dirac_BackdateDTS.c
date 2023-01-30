/* backdate the list [p_block .. p_block->p_next where p_next == p_last] */
static void dirac_BackdateDTS( block_t *p_block, block_t *p_last, date_t *p_dts )
{
    /* Transverse p_last backwards.  (no p_prev anymore) */
    block_t **pp_array = NULL;
    int n = block_ChainToArray( p_block, &pp_array );
    while( n ) if( pp_array[--n] == p_last ) break;
    /* want to start at p_last->p_prev */
    while( n-- )
    {
        if( pp_array[n]->i_flags & DIRAC_NON_DATED )
            continue;
        if( pp_array[n]->i_dts <= VLC_TS_INVALID )
            pp_array[n]->i_dts = date_Decrement( p_dts, 1 );
    }
    free( pp_array );
}
