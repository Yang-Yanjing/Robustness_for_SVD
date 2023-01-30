}
void vout_SetDisplayZoom(vout_display_t *vd, unsigned num, unsigned den)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    if (num > 0 && den > 0) {
        vlc_ureduce(&num, &den, num, den, 0);
    } else {
        num = 1;
        den = 1;
    }
    if (osys->is_display_filled ||
        osys->zoom.num != num || osys->zoom.den != den) {
        osys->ch_zoom = true;
        osys->zoom.num = num;
        osys->zoom.den = den;
    }
}
