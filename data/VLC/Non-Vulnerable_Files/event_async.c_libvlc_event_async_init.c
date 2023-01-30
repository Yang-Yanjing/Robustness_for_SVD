static void
libvlc_event_async_init(libvlc_event_manager_t * p_em)
{
    p_em->async_event_queue = calloc(1, sizeof(struct libvlc_event_async_queue));
    int error = vlc_threadvar_create(&queue(p_em)->is_asynch_dispatch_thread_var, NULL);
    assert(!error);
    vlc_mutex_init(&queue(p_em)->lock);
    vlc_cond_init(&queue(p_em)->signal);
    vlc_cond_init(&queue(p_em)->signal_idle);
    error = vlc_clone (&queue(p_em)->thread, event_async_loop, p_em, VLC_THREAD_PRIORITY_LOW);
    if(error)
    {
        free(p_em->async_event_queue);
        p_em->async_event_queue = NULL;
        return;
    }
}
