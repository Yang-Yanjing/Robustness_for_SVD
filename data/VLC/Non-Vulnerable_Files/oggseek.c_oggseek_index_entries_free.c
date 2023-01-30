/* free all entries in index list */
void oggseek_index_entries_free ( demux_index_entry_t *idx )
{
    demux_index_entry_t *idx_next;
    while ( idx != NULL )
    {
        idx_next = idx->p_next;
        free( idx );
        idx = idx_next;
    }
}
