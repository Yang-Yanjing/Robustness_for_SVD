}
static void SwapBuffers (vlc_gl_t *gl)
{
    vlc_gl_sys_t *sys = gl->sys;
    eglSwapBuffers (sys->display, sys->surface);
}
