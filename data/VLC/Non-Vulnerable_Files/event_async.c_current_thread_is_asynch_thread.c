}
static inline bool current_thread_is_asynch_thread(libvlc_event_manager_t * p_em)
{
    return vlc_threadvar_get(queue(p_em)->is_asynch_dispatch_thread_var)
            != NULL;
}
