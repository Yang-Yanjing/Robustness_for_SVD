}
int QueueEnqueue( queue_t *p_queue, command_t *p_cmd )
{
    if( p_queue->p_tail != NULL )
    {
        p_queue->p_tail->p_next = p_cmd;
    }
    if( p_queue->p_head == NULL )
    {
        p_queue->p_head = p_cmd;
    }
    p_queue->p_tail = p_cmd;
    p_cmd->p_next = NULL;
    return VLC_SUCCESS;
}
