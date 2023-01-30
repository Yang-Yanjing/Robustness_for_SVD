}
static void Swap(vlc_gl_t *gl)
{
    vout_display_t *vd = gl->sys;
    SwapBuffers(vd->sys->hGLDC);
}
