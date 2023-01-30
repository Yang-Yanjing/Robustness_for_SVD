/* */
bool vout_snapshot_IsRequested(vout_snapshot_t *snap)
{
    bool has_request = false;
    if (!vlc_mutex_trylock(&snap->lock)) {
        has_request = snap->request_count > 0;
        vlc_mutex_unlock(&snap->lock);
    }
    return has_request;
}
