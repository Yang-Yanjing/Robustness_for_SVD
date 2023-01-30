/* */
void vout_snapshot_Init(vout_snapshot_t *snap)
{
    vlc_mutex_init(&snap->lock);
    vlc_cond_init(&snap->wait);
    snap->is_available = true;
    snap->request_count = 0;
    snap->picture = NULL;
}
