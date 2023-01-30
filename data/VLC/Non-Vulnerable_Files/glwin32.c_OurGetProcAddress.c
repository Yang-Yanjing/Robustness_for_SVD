}
static void *OurGetProcAddress(vlc_gl_t *gl, const char *name)
{
    VLC_UNUSED(gl);
    return wglGetProcAddress(name);
}
