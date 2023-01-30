}
static int Control(vout_display_t *vd, int query, va_list args)
{
    switch (query) {
    case VOUT_DISPLAY_GET_OPENGL: {
        vlc_gl_t **gl = va_arg(args, vlc_gl_t **);
        *gl = &vd->sys->gl;
        CommonDisplay(vd);
        return VLC_SUCCESS;
    }
    default:
        return CommonControl(vd, query, args);
    }
}
