 */
static int Control(vout_display_t *vd, int query, va_list args)
{
    vout_display_sys_t *sys = vd->sys;
    switch (query) {
    case VOUT_DISPLAY_HIDE_MOUSE:
        caca_set_mouse(sys->dp, 0);
        return VLC_SUCCESS;
    case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE: {
        const vout_display_cfg_t *cfg = va_arg(args, const vout_display_cfg_t *);
        caca_refresh_display(sys->dp);
        /* Not quite good but not sure how to resize it */
        if ((int)cfg->display.width  != caca_get_display_width(sys->dp) ||
            (int)cfg->display.height != caca_get_display_height(sys->dp))
            return VLC_EGENERIC;
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_CHANGE_ZOOM:
    case VOUT_DISPLAY_CHANGE_DISPLAY_FILLED:
    case VOUT_DISPLAY_CHANGE_SOURCE_ASPECT:
        return VLC_EGENERIC;
    case VOUT_DISPLAY_CHANGE_SOURCE_CROP:
        if (sys->dither)
            cucul_free_dither(sys->dither);
        sys->dither = NULL;
        return VLC_SUCCESS;
    default:
        msg_Err(vd, "Unsupported query in vout display caca");
        return VLC_EGENERIC;
    }
}
