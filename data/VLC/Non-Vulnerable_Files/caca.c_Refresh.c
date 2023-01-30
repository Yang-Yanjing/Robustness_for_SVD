 */
static void Refresh(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    /* */
    caca_refresh_display(sys->dp);
    /* */
    const unsigned width  = caca_get_display_width(sys->dp);
    const unsigned height = caca_get_display_height(sys->dp);
    if (width  != vd->cfg->display.width ||
        height != vd->cfg->display.height)
        vout_display_SendEventDisplaySize(vd, width, height, false);
}
