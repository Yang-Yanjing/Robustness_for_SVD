/* */
picture_t *vout_snapshot_Get(vout_snapshot_t *snap, mtime_t timeout)
{
    vlc_mutex_lock(&snap->lock);
    /* */
    snap->request_count++;
    /* */
    const mtime_t deadline = mdate() + timeout;
    while (snap->is_available && !snap->picture && mdate() < deadline)
        vlc_cond_timedwait(&snap->wait, &snap->lock, deadline);
    /* */
    picture_t *picture = snap->picture;
    if (picture)
        snap->picture = picture->p_next;
    else if (snap->request_count > 0)
        snap->request_count--;
    vlc_mutex_unlock(&snap->lock);
    return picture;
}
