void
libvlc_event_async_ensure_listener_removal(libvlc_event_manager_t * p_em, libvlc_event_listener_t * listener)
{
    if(!is_queue_initialized(p_em)) return;
    queue_lock(p_em);
    pop_listener(p_em, listener);
    // Wait for the asynch_loop to have processed all events.
    if(!current_thread_is_asynch_thread(p_em))
    {
        while(!queue(p_em)->is_idle)
            vlc_cond_wait(&queue(p_em)->signal_idle, &queue(p_em)->lock);
    }
    queue_unlock(p_em);
}
