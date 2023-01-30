}
static int OpenGL (vlc_object_t *obj)
{
    static const struct gl_api api = {
        "OpenGL", EGL_OPENGL_API, 4, EGL_OPENGL_BIT,
        { EGL_NONE },
    };
    return Open (obj, &api);
}
