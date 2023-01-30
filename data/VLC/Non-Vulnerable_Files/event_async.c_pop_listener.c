/* Lock must be held */
static void pop_listener(libvlc_event_manager_t * p_em, libvlc_event_listener_t * listener)
{
    struct queue_elmt * iter = queue(p_em)->first_elmt;
    struct queue_elmt * prev = NULL;
    while (iter) {
        if(listeners_are_equal(&iter->listener, listener))
        {
            struct queue_elmt * to_delete = iter;
            if(!prev)
                queue(p_em)->first_elmt = to_delete->next;
            else
                prev->next = to_delete->next;
            iter = to_delete->next;
            free(to_delete);
        }
        else {
            prev = iter;
            iter = iter->next;
        }
    }
    queue(p_em)->last_elmt=prev;
}
