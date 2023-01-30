}
static inline bool is_queue_initialized(libvlc_event_manager_t * p_em)
{
    return queue(p_em) != NULL;
}
