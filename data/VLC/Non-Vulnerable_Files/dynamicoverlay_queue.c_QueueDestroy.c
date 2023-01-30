}
int QueueDestroy( queue_t *p_queue )
{
    command_t *p_cur = p_queue->p_head, *p_temp;
    while( p_cur != NULL )
    {
        p_temp = p_cur;
        p_cur = p_cur->p_next;
        free( p_temp );
    }
    p_queue->p_head = NULL;
    p_queue->p_tail = NULL;
    return VLC_SUCCESS;
}
