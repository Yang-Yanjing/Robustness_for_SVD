}
static int OpenGLES (vlc_object_t *obj)
{
    static const struct gl_api api = {
        "OpenGL_ES", EGL_OPENGL_ES_API, 0, EGL_OPENGL_ES_BIT,
        { EGL_CONTEXT_CLIENT_VERSION, 1, EGL_NONE },
    };
    return Open (obj, &api);
}
