/* Lock must be held */
static bool pop(libvlc_event_manager_t * p_em,
                libvlc_event_listener_t * listener, libvlc_event_t * event)
{
    if(!queue(p_em)->first_elmt)
        return false; /* No first_elmt */
    struct queue_elmt * elmt = queue(p_em)->first_elmt;
    *listener = elmt->listener;
    *event = elmt->event;
    queue(p_em)->first_elmt = elmt->next;
    if( !elmt->next ) queue(p_em)->last_elmt=NULL;
    free(elmt);
    return true;
}
