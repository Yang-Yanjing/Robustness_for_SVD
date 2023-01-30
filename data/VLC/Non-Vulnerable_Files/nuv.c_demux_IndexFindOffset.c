}
static int64_t demux_IndexFindOffset( demux_index_t *p_idx, int64_t i_offset )
{
    int i_min = 0;
    int i_max = p_idx->i_idx-1;
    /* Empty index */
    if( p_idx->i_idx <= 0 )
        return -1;
    /* Special border case */
    if( i_offset <= p_idx->idx[0].i_offset )
        return p_idx->idx[0].i_offset;
    if( i_offset == p_idx->idx[i_max].i_offset )
        return p_idx->idx[i_max].i_offset;
    if( i_offset > p_idx->idx[i_max].i_offset )
        return -1;
    /* Dicho */
    for( ;; )
    {
        int i_med;
        if( i_max - i_min <= 1 )
            break;
        i_med = (i_min+i_max)/2;
        if( p_idx->idx[i_med].i_offset < i_offset )
            i_min = i_med;
        else if( p_idx->idx[i_med].i_offset > i_offset )
            i_max = i_med;
        else
            return p_idx->idx[i_med].i_offset;
    }
    /* return nearest */
    if( i_offset - p_idx->idx[i_min].i_offset < p_idx->idx[i_max].i_offset - i_offset )
        return p_idx->idx[i_min].i_offset;
    else
        return p_idx->idx[i_max].i_offset;
}
