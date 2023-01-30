 */
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
    /* Allocate structure */
    vd->sys = sys = calloc(1, sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    /* */
    if (CommonInit(vd))
        goto error;
    EventThreadUpdateTitle(sys->event, VOUT_TITLE " (OpenGL output)");
    /* process selected GPU affinity */
    int nVidiaAffinity = var_InheritInteger(vd, "gpu-affinity");
    if (nVidiaAffinity >= 0) CreateGPUAffinityDC(vd, nVidiaAffinity);
    /* */
    sys->hGLDC = GetDC(sys->hvideownd);
    /* Set the pixel format for the DC */
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    SetPixelFormat(sys->hGLDC,
                   ChoosePixelFormat(sys->hGLDC, &pfd), &pfd);
    /*
     * Create and enable the render context
     * For GPU affinity, attach the window DC
     * to the GPU affinity DC
     */
    sys->hGLRC = wglCreateContext((sys->affinityHDC != NULL) ? sys->affinityHDC : sys->hGLDC);
    wglMakeCurrent(sys->hGLDC, sys->hGLRC);
    const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
#ifdef WGL_EXT_swap_control
    if (HasExtension(extensions, "WGL_EXT_swap_control")) {
        PFNWGLSWAPINTERVALEXTPROC SwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
        if (SwapIntervalEXT)
            SwapIntervalEXT(1);
    }
#endif
    /* */
    sys->gl.lock = NULL;
    sys->gl.unlock = NULL;
    sys->gl.swap = Swap;
    sys->gl.getProcAddress = OurGetProcAddress;
    sys->gl.sys = vd;
    video_format_t fmt = vd->fmt;
    const vlc_fourcc_t *subpicture_chromas;
    sys->vgl = vout_display_opengl_New(&fmt, &subpicture_chromas, &sys->gl);
    if (!sys->vgl)
        goto error;
    vout_display_info_t info = vd->info;
    info.has_double_click = true;
    info.has_hide_mouse = false;
    info.has_event_thread = true;
    info.subpicture_chromas = subpicture_chromas;
   /* Setup vout_display now that everything is fine */
    vd->fmt  = fmt;
    vd->info = info;
    vd->pool    = Pool;
    vd->prepare = Prepare;
    vd->display = Display;
    vd->control = Control;
    vd->manage  = Manage;
    return VLC_SUCCESS;
error:
    Close(object);
    return VLC_EGENERIC;
}
