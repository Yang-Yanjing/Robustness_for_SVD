   time stamps) */
const demux_index_entry_t *OggSeek_IndexAdd ( logical_stream_t *p_stream,
                                             int64_t i_timestamp,
                                             int64_t i_pagepos )
{
    demux_index_entry_t *idx;
    demux_index_entry_t *oidx;
    demux_index_entry_t *last_idx = NULL;
    if ( p_stream == NULL ) return NULL;
    oidx = idx = p_stream->idx;
    if ( i_timestamp < 1 || i_pagepos < 1 ) return NULL;
    if ( idx == NULL )
    {
        demux_index_entry_t *ie = index_entry_new();
        if ( !ie ) return NULL;
        ie->i_value = i_timestamp;
        ie->i_pagepos = i_pagepos;
        p_stream->idx = ie;
        return ie;
    }
    while ( idx != NULL )
    {
        if ( idx->i_pagepos > i_pagepos ) break;
        last_idx = idx;
        idx = idx->p_next;
    }
    /* new entry; insert after last_idx */
    idx = index_entry_new();
    if ( !idx ) return NULL;
    if ( last_idx != NULL )
    {
        idx->p_next = last_idx->p_next;
        last_idx->p_next = idx;
        idx->p_prev = last_idx;
    }
    else
    {
        idx->p_next = oidx;
        oidx = idx;
    }
    if ( idx->p_next != NULL )
    {
        idx->p_next->p_prev = idx;
    }
    idx->i_value = i_timestamp;
    idx->i_pagepos = i_pagepos;
    return idx;
}
