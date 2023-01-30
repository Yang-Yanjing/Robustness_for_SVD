}
static int OpenGLES2 (vlc_object_t *obj)
{
    static const struct gl_api api = {
        "OpenGL_ES", EGL_OPENGL_ES_API, 3, EGL_OPENGL_ES2_BIT,
        { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE },
    };
    return Open (obj, &api);
}
