}
void vout_SetWindowState(vout_display_t *vd, unsigned state)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    vlc_mutex_lock(&osys->lock);
    if (osys->wm_state != state) {
        osys->ch_wm_state = true;
        osys->wm_state = state;
    }
    vlc_mutex_unlock(&osys->lock);
}
