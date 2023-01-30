/* Create an GPU Affinity DC */
static void CreateGPUAffinityDC(vout_display_t *vd, UINT nVidiaAffinity) {
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
    PFNWGLENUMGPUSNVPROC fncEnumGpusNV = (PFNWGLENUMGPUSNVPROC)wglGetProcAddress("wglEnumGpusNV");
    PFNWGLCREATEAFFINITYDCNVPROC fncCreateAffinityDCNV = (PFNWGLCREATEAFFINITYDCNVPROC)wglGetProcAddress("wglCreateAffinityDCNV");
    /* delete the temporary GL context */
    wglDeleteContext(hGLRC);
    /* see if we have the extensions */
    if (!fncEnumGpusNV || !fncCreateAffinityDCNV) return;
    /* find the graphics card */
    HGPUNV GpuMask[2];
    GpuMask[0] = NULL;
    GpuMask[1] = NULL;
    HGPUNV hGPU;
    if (!fncEnumGpusNV(nVidiaAffinity, &hGPU)) return;
    /* make the affinity DC */
    GpuMask[0] = hGPU;
    vd->sys->affinityHDC = fncCreateAffinityDCNV(GpuMask);
    if (vd->sys->affinityHDC == NULL) return;
    SetPixelFormat(vd->sys->affinityHDC,
        ChoosePixelFormat(vd->sys->affinityHDC, &pfd), &pfd);
    msg_Dbg( vd, "GPU affinity set to adapter: %d",
                     nVidiaAffinity );
}
