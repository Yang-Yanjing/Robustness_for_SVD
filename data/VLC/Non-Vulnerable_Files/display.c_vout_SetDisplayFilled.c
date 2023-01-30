}
void vout_SetDisplayFilled(vout_display_t *vd, bool is_filled)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    if (!osys->is_display_filled != !is_filled) {
        osys->ch_display_filled = true;
        osys->is_display_filled = is_filled;
    }
}
