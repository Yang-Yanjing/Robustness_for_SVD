}
static void dirac_ReorderDequeueAndReleaseBlock( decoder_t *p_dec, block_t *p_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    /* Check if that block is present in reorder queue and release it
       if needed */
    struct dirac_reorder_entry **pp_at = &p_sys->reorder_buf.p_head;
    for( ; *pp_at; pp_at = &(*pp_at)->p_next )
    {
        /* backup address in case we remove member */
        struct dirac_reorder_entry *p_entry = *pp_at;
        if ( p_entry->p_eu == p_block )
        {
            /* unlink member */
            *pp_at = (*pp_at)->p_next;
            /* Add to empty reorder entry list*/
            p_entry->p_next = p_sys->reorder_buf.p_empty;
            p_sys->reorder_buf.p_empty = p_entry;
            p_sys->reorder_buf.u_size--;
            break;
        }
    }
    block_Release( p_block );
}
