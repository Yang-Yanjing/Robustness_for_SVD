/* simulate the dirac picture reorder buffer */
static block_t *dirac_Reorder( decoder_t *p_dec, block_t *p_block_in, uint32_t u_picnum )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( !p_sys->reorder_buf.u_size_max )
        /* reorder buffer disabled */
        return p_block_in;
    /* Modeling the reorder buffer:
     * 1. If the reorder buffer is not full, insert picture for reordering.
     *    No picture is output by the system this picture period
     * 2. If the reorder buffer is full:
     *    a. The picture decoded this period (u_picnum) bypasses the reorder
     *       buffer if it has a lower picture number than any entry in the
     *       reorder buffer. This picture is output by the system.
     *    b. Otherwise, the lowest picture number in the reorder buffer is
     *       removed from the buffer and output by the system.  The current
     *       decoded picture (u_picnum) is inserted into the reorder buffer
     */
    block_t *p_block = NULL;
    /* Determine if the picture needs to be inserted */
    if( p_sys->reorder_buf.u_size == p_sys->reorder_buf.u_size_max )
    {
        /* (2) reorder buffer is full */
        if( !p_sys->reorder_buf.u_size_max ||
            dirac_PictureNbeforeM( u_picnum, p_sys->reorder_buf.p_head->u_picnum ) )
        {
            /* (2a) current picture is first in order */
            return p_block_in;
        }
        /* (2b) extract the youngest picture in the buffer */
        p_block = p_sys->reorder_buf.p_head->p_eu;
        struct dirac_reorder_entry *p_tmp = p_sys->reorder_buf.p_head;
        p_sys->reorder_buf.p_head = p_tmp->p_next;
        p_tmp->p_next = p_sys->reorder_buf.p_empty;
        p_sys->reorder_buf.p_empty = p_tmp;
        p_sys->reorder_buf.u_size--;
    }
    /* (1) and (2b) both require u_picnum to be inserted */
    struct dirac_reorder_entry *p_current = p_sys->reorder_buf.p_empty;
    p_sys->reorder_buf.p_empty = p_current->p_next;
    p_sys->reorder_buf.u_size++;
    /* insertion sort to keep p_head always sorted, earliest first */
    struct dirac_reorder_entry **pp_at = &p_sys->reorder_buf.p_head;
    for( ; *pp_at; pp_at = &(*pp_at)->p_next )
        if( dirac_PictureNbeforeM( u_picnum, (*pp_at)->u_picnum ) )
            break;
    p_current->u_picnum = u_picnum;
    p_current->p_eu = p_block_in;
    p_current->p_next = *pp_at;
    *pp_at = p_current;
    return p_block;
}
