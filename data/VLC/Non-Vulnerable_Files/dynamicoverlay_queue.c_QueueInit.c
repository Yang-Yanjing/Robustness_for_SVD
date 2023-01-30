 *****************************************************************************/
int QueueInit( queue_t *p_queue )
{
    memset( p_queue, 0, sizeof( queue_t ) );
    p_queue->p_head = NULL;
    p_queue->p_tail = NULL;
    return VLC_SUCCESS;
}
