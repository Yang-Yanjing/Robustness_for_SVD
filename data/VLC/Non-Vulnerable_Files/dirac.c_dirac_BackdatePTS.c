/* backdate the list [p_block .. p_block->p_next where p_next == p_last] */
static void dirac_BackdatePTS( block_t *p_block, block_t *p_last, date_t *p_pts, uint32_t u_pts_picnum )
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
        if( pp_array[n]->i_dts > VLC_TS_INVALID )
            continue;
        dirac_block_encap_t *dbe = dirac_GetBlockEncap( pp_array[n] );
        int32_t u_pic_num = dbe ? dbe->u_picture_number : 0;
        int32_t i_dist = u_pic_num - u_pts_picnum;
        date_t pts = *p_pts;
        if( i_dist >= 0 )
            pp_array[n]->i_pts = date_Increment( &pts, i_dist );
        else
            pp_array[n]->i_pts = date_Decrement( &pts, -i_dist );
    }
    free( pp_array );
}
