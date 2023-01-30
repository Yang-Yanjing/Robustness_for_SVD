}
static bool CheckAPI (EGLDisplay dpy, const char *api)
{
    const char *apis = eglQueryString (dpy, EGL_CLIENT_APIS);
    return CheckToken(apis, api);
}
