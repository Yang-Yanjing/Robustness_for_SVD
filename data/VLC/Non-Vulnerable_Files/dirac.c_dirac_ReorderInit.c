 */
static void dirac_ReorderInit( struct dirac_reorder_buffer *p_rb )
{
    memset( p_rb, 0, sizeof(*p_rb) );
    p_rb->u_size_max = 2;
    p_rb->p_empty = p_rb->p_entries;
    p_rb->p_entries[31].p_next = NULL;
    for( int i = 0; i < 31; i++ )
        p_rb->p_entries[i].p_next = &p_rb->p_entries[i+1];
}
