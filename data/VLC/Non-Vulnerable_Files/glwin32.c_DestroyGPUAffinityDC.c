/* Destroy an GPU Affinity DC */
static void DestroyGPUAffinityDC(vout_display_t *vd) {
    if (vd->sys->affinityHDC == NULL) return;
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    /* create a temporary GL context */
    HDC winDC = GetDC(vd->sys->hvideownd);
    SetPixelFormat(winDC, ChoosePixelFormat(winDC, &pfd), &pfd);
    HGLRC hGLRC = wglCreateContext(winDC);
    wglMakeCurrent(winDC, hGLRC);
    /* Initialize the neccessary function pointers */
    PFNWGLDELETEDCNVPROC fncDeleteDCNV = (PFNWGLDELETEDCNVPROC)wglGetProcAddress("wglDeleteDCNV");
    /* delete the temporary GL context */
    wglDeleteContext(hGLRC);
    /* see if we have the extensions */
    if (!fncDeleteDCNV) return;
    /* delete the affinity DC */
    fncDeleteDCNV(vd->sys->affinityHDC);
}
