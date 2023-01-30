}
static inline void queue_unlock(libvlc_event_manager_t * p_em)
{
    vlc_mutex_unlock(&queue(p_em)->lock);
}
