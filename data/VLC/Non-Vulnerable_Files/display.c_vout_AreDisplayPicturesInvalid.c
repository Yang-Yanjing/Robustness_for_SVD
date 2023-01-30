}
bool vout_AreDisplayPicturesInvalid(vout_display_t *vd)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    vlc_mutex_lock(&osys->lock);
    const bool reset_pictures = osys->reset_pictures;
    vlc_mutex_unlock(&osys->lock);
    return reset_pictures;
}
