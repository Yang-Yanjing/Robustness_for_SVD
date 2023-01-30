}
static int MakeCurrent (vlc_gl_t *gl)
{
    vlc_gl_sys_t *sys = gl->sys;
    if (eglMakeCurrent (sys->display, sys->surface, sys->surface,
                        sys->context) != EGL_TRUE)
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
