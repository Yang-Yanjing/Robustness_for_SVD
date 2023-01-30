}
int QueueTransfer( queue_t *p_sink, queue_t *p_source )
{
    if( p_source->p_head == NULL ) {
        return VLC_SUCCESS;
    }
    if( p_sink->p_head == NULL ) {
        p_sink->p_head = p_source->p_head;
        p_sink->p_tail = p_source->p_tail;
    } else {
        p_sink->p_tail->p_next = p_source->p_head;
        p_sink->p_tail = p_source->p_tail;
    }
    p_source->p_head = p_source->p_tail = NULL;
    return VLC_SUCCESS;
}
