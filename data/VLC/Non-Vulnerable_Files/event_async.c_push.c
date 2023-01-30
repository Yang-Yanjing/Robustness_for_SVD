/* Lock must be held */
static void push(libvlc_event_manager_t * p_em,
                 libvlc_event_listener_t * listener, libvlc_event_t * event)
{
    struct queue_elmt * elmt = malloc(sizeof(struct queue_elmt));
    elmt->listener = *listener;
    elmt->event = *event;
    elmt->next = NULL;
    /* Append to the end of the queue */
    if(!queue(p_em)->first_elmt)
        queue(p_em)->first_elmt = elmt;
    else
        queue(p_em)->last_elmt->next = elmt;
    queue(p_em)->last_elmt = elmt;
}
