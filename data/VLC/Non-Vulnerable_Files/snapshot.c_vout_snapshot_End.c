}
void vout_snapshot_End(vout_snapshot_t *snap)
{
    vlc_mutex_lock(&snap->lock);
    snap->is_available = false;
    vlc_cond_broadcast(&snap->wait);
    vlc_mutex_unlock(&snap->lock);
}
