}
void vout_SetDisplayAspect(vout_display_t *vd, unsigned dar_num, unsigned dar_den)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    unsigned sar_num, sar_den;
    if (dar_num > 0 && dar_den > 0) {
        sar_num = dar_num * osys->source.i_visible_height;
        sar_den = dar_den * osys->source.i_visible_width;
        vlc_ureduce(&sar_num, &sar_den, sar_num, sar_den, 0);
    } else {
        sar_num = 0;
        sar_den = 0;
    }
    if (osys->sar.num != sar_num || osys->sar.den != sar_den) {
        osys->ch_sar = true;
        osys->sar.num = sar_num;
        osys->sar.den = sar_den;
    }
}
