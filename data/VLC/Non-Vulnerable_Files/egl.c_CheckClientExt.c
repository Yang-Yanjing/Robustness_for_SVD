}
static bool CheckClientExt(const char *name)
{
    const char *exts = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    return CheckToken(exts, name);
}
