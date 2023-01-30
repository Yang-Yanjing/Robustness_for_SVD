}
static int Control(vout_display_t *vd, int query, va_list args)
{
    vout_display_sys_t *sys = vd->sys;
    switch (query) {
    case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE: {
        const vout_display_cfg_t *cfg = va_arg(args, const vout_display_cfg_t *);
        if (cfg->display.width  != sys->width ||
            cfg->display.height != sys->height)
            return VLC_EGENERIC;
        return VLC_SUCCESS;
    }
    default:
        msg_Err(vd, "Unsupported query in vout display fb");
        return VLC_EGENERIC;
    }
}
