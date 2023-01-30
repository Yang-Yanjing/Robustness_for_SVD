 **************************************************************************/
static void * event_async_loop(void * arg)
{
    libvlc_event_manager_t * p_em = arg;
    libvlc_event_listener_t listener;
    libvlc_event_t event;
    vlc_threadvar_set(queue(p_em)->is_asynch_dispatch_thread_var, p_em);
    queue_lock(p_em);
    while (true) {
        int has_listener = pop(p_em, &listener, &event);
        if (has_listener)
        {
            queue_unlock(p_em);
            listener.pf_callback(&event, listener.p_user_data); // This might edit the queue
            queue_lock(p_em);
        }
        else
        {
            queue(p_em)->is_idle = true;
            mutex_cleanup_push(&queue(p_em)->lock);
            vlc_cond_broadcast(&queue(p_em)->signal_idle); // We'll be idle
            vlc_cond_wait(&queue(p_em)->signal, &queue(p_em)->lock);
            vlc_cleanup_pop();
            queue(p_em)->is_idle = false;
        }
    }
    queue_unlock(p_em);
    return NULL;
}
