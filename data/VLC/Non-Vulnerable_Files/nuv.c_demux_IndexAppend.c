}
static void demux_IndexAppend( demux_index_t *p_idx,
                               int64_t i_time, int64_t i_offset )
{
    /* Be sure to append new entry (we don't insert point) */
    if( p_idx->i_idx > 0 && p_idx->idx[p_idx->i_idx-1].i_time >= i_time )
        return;
    /* */
    if( p_idx->i_idx >= p_idx->i_idx_max )
    {
        if( p_idx->i_idx >= DEMUX_INDEX_SIZE_MAX )
        {
            /* Avoid too big index */
            const int64_t i_length = p_idx->idx[p_idx->i_idx-1].i_time -
                                                        p_idx->idx[0].i_time;
            const int i_count = DEMUX_INDEX_SIZE_MAX/2;
            int i, j;
            /* We try to reduce the resolution of the index by a factor 2 */
            for( i = 1, j = 1; i < p_idx->i_idx; i++ )
            {
                if( p_idx->idx[i].i_time < j * i_length / i_count )
                    continue;
                p_idx->idx[j++] = p_idx->idx[i];
            }
            p_idx->i_idx = j;
            if( p_idx->i_idx > 3 * DEMUX_INDEX_SIZE_MAX / 4 )
            {
                /* We haven't created enough space
                 * (This method won't create a good index but work for sure) */
                for( i = 0; i < p_idx->i_idx/2; i++ )
                    p_idx->idx[i] = p_idx->idx[2*i];
                p_idx->i_idx /= 2;
            }
        }
        else
        {
            p_idx->i_idx_max += 1000;
            p_idx->idx = xrealloc( p_idx->idx,
                                p_idx->i_idx_max*sizeof(demux_index_entry_t));
        }
    }
    /* */
    p_idx->idx[p_idx->i_idx].i_time = i_time;
    p_idx->idx[p_idx->i_idx].i_offset = i_offset;
    p_idx->i_idx++;
}
