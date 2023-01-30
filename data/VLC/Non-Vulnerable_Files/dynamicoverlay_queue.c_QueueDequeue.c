}
command_t *QueueDequeue( queue_t *p_queue )
{
    if( p_queue->p_head == NULL )
    {
        return NULL;
    }
    else
    {
        command_t *p_ret = p_queue->p_head;
        if( p_queue->p_head == p_queue->p_tail )
        {
            p_queue->p_head = p_queue->p_tail = NULL;
        }
        else
        {
            p_queue->p_head = p_queue->p_head->p_next;
        }
        return p_ret;
    }
}
