}
static int Control(vout_display_t *vd, int query, va_list args)
{
    switch (query) {
    case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE: {
        const vout_display_cfg_t *cfg = va_arg(args, const vout_display_cfg_t *);
        if (cfg->display.width  != vd->fmt.i_width ||
            cfg->display.height != vd->fmt.i_height)
            return VLC_EGENERIC;
        return VLC_SUCCESS;
    }
    default:
        msg_Err(vd, "Unsupported query in vout display directfb");
        return VLC_EGENERIC;
    }
}
