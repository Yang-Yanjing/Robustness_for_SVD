}
void vout_snapshot_Set(vout_snapshot_t *snap,
                       const video_format_t *fmt,
                       const picture_t *picture)
{
    if (!fmt)
        fmt = &picture->format;
    vlc_mutex_lock(&snap->lock);
    while (snap->request_count > 0) {
        picture_t *dup = picture_NewFromFormat(fmt);
        if (!dup)
            break;
        picture_Copy(dup, picture);
        dup->p_next = snap->picture;
        snap->picture = dup;
        snap->request_count--;
    }
    vlc_cond_broadcast(&snap->wait);
    vlc_mutex_unlock(&snap->lock);
}
