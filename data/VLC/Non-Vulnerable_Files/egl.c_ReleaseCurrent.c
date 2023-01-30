}
static void ReleaseCurrent (vlc_gl_t *gl)
{
    vlc_gl_sys_t *sys = gl->sys;
    eglMakeCurrent (sys->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                    EGL_NO_CONTEXT);
}
