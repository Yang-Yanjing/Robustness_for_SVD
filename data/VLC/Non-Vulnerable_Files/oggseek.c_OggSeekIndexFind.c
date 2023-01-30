}
static bool OggSeekIndexFind ( logical_stream_t *p_stream, int64_t i_timestamp,
                               int64_t *pi_pos_lower, int64_t *pi_pos_upper )
{
    demux_index_entry_t *idx = p_stream->idx;
    while ( idx != NULL )
    {
        if ( idx->i_value <= i_timestamp )
        {
            if ( !idx->p_next ) /* found on last index */
            {
                *pi_pos_lower = idx->i_pagepos;
                return true;
            }
            if ( idx->p_next->i_value > i_timestamp )
            {
                *pi_pos_lower = idx->i_pagepos;
                *pi_pos_upper = idx->p_next->i_pagepos;
                return true;
            }
        }
        idx = idx->p_next;
    }
    return false;
}
