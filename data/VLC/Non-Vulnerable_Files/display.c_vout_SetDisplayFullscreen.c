}
void vout_SetDisplayFullscreen(vout_display_t *vd, bool is_fullscreen)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    vlc_mutex_lock(&osys->lock);
    if (!osys->is_fullscreen != !is_fullscreen) {
        osys->ch_fullscreen = true;
        osys->is_fullscreen = is_fullscreen;
    }
    vlc_mutex_unlock(&osys->lock);
}
