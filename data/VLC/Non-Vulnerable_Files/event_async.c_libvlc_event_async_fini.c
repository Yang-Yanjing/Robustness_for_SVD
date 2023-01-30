void
libvlc_event_async_fini(libvlc_event_manager_t * p_em)
{
    if(!is_queue_initialized(p_em)) return;
    if(current_thread_is_asynch_thread(p_em))
    {
        fprintf(stderr, "*** Error: releasing the last reference of the observed object from its callback thread is not (yet!) supported\n");
        abort();
    }
    vlc_thread_t thread = queue(p_em)->thread;
    if(thread)
    {
        vlc_cancel(thread);
        vlc_join(thread, NULL);
    }
    vlc_mutex_destroy(&queue(p_em)->lock);
    vlc_cond_destroy(&queue(p_em)->signal);
    vlc_cond_destroy(&queue(p_em)->signal_idle);
    vlc_threadvar_delete(&queue(p_em)->is_asynch_dispatch_thread_var);
    struct queue_elmt * iter = queue(p_em)->first_elmt;
    while (iter) {
        struct queue_elmt * elemt_to_delete = iter;
        iter = iter->next;
        free(elemt_to_delete);
    }
    free(queue(p_em));
}
