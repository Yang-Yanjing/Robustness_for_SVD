}
static void Close (vlc_object_t *obj)
{
    vlc_gl_t *gl = (vlc_gl_t *)obj;
    vlc_gl_sys_t *sys = gl->sys;
    if (sys->display != EGL_NO_DISPLAY)
    {
        if (sys->surface != EGL_NO_SURFACE)
            eglDestroySurface(sys->display, sys->surface);
        eglTerminate(sys->display);
    }
#ifdef USE_PLATFORM_X11
    if (sys->x11 != NULL)
        XCloseDisplay(sys->x11);
#endif
    free (sys);
}
