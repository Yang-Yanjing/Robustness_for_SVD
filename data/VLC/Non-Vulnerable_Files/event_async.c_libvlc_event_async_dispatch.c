void
libvlc_event_async_dispatch(libvlc_event_manager_t * p_em, libvlc_event_listener_t * listener, libvlc_event_t * event)
{
    // We do a lazy init here, to prevent constructing the thread when not needed.
    vlc_mutex_lock(&p_em->object_lock);
    if(!queue(p_em))
        libvlc_event_async_init(p_em);
    vlc_mutex_unlock(&p_em->object_lock);
    queue_lock(p_em);
    push(p_em, listener, event);
    vlc_cond_signal(&queue(p_em)->signal);
    queue_unlock(p_em);
}
