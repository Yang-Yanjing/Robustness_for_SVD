/* internal function to create a new list member */
static demux_index_entry_t *index_entry_new( void )
{
    demux_index_entry_t *idx = xmalloc( sizeof( demux_index_entry_t ) );
    if ( !idx ) return NULL;
    idx->p_next = idx->p_prev = NULL;
    idx->i_pagepos_end = -1;
    return idx;
}
