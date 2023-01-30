 */
static void Close(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = vd->sys;
    if (sys->vgl)
        vout_display_opengl_Delete(sys->vgl);
    if (sys->hGLDC && sys->hGLRC)
        wglMakeCurrent(NULL, NULL);
    if (sys->hGLRC)
        wglDeleteContext(sys->hGLRC);
    if (sys->hGLDC)
        ReleaseDC(sys->hvideownd, sys->hGLDC);
    DestroyGPUAffinityDC(vd);
    CommonClean(vd);
    free(sys);
}
