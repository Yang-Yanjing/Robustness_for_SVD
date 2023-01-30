}
static inline void queue_lock(libvlc_event_manager_t * p_em)
{
    vlc_mutex_lock(&queue(p_em)->lock);
}
