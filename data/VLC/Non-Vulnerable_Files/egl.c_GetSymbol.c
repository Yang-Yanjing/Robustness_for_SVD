}
static void *GetSymbol(vlc_gl_t *gl, const char *procname)
{
    (void) gl;
    return (void *)eglGetProcAddress (procname);
}
