static void*  event_async_loop(void * arg);
static inline struct libvlc_event_async_queue * queue(libvlc_event_manager_t * p_em)
{
    return p_em->async_event_queue;
}
