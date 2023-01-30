}
void vout_snapshot_Clean(vout_snapshot_t *snap)
{
    picture_t *picture = snap->picture;
    while (picture) {
        picture_t *next = picture->p_next;
        picture_Release(picture);
        picture = next;
    }
    vlc_cond_destroy(&snap->wait);
    vlc_mutex_destroy(&snap->lock);
}
