 *****************************************************************************/
static int64_t GetMplsUniqueDuration( const bd_mpls_t *p_mpls )
{
    int64_t i_length = 0;
    for( int i = 0; i < p_mpls->i_play_item; i++ )
    {
        const bd_mpls_play_item_t *p_item0 = &p_mpls->p_play_item[i];
        int j;
        for( j = i+1; j < p_mpls->i_play_item; j++ )
        {
            const bd_mpls_play_item_t *p_item1 = &p_mpls->p_play_item[j];
            if( p_item0->clpi.i_id == p_item1->clpi.i_id &&
                p_item0->clpi.i_stc_id == p_item1->clpi.i_stc_id &&
                p_item0->i_in_time == p_item1->i_in_time &&
                p_item0->i_out_time == p_item1->i_out_time )
                break;
        }
        if( j >= p_mpls->i_play_item )
            i_length += p_item0->i_out_time - p_item0->i_in_time;
    }
    return i_length;
}
