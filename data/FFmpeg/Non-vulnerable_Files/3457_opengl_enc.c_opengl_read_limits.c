static av_cold int opengl_read_limits(OpenGLContext *opengl)
{
    static const struct{
        const char *extension;
        int major;
        int minor;
    } required_extensions[] = {
        { "GL_ARB_multitexture",         1, 3 },
        { "GL_ARB_vertex_buffer_object", 1, 5 }, 
        { "GL_ARB_vertex_shader",        2, 0 },
        { "GL_ARB_fragment_shader",      2, 0 },
        { "GL_ARB_shader_objects",       2, 0 },
        { NULL,                          0, 0 }
    };
    int i, major, minor;
    const char *extensions, *version;
    version = glGetString(GL_VERSION);
    extensions = glGetString(GL_EXTENSIONS);
    av_log(opengl, AV_LOG_DEBUG, "OpenGL version: %s\n", version);
    sscanf(version, "%d.%d", &major, &minor);
    for (i = 0; required_extensions[i].extension; i++) {
        if (major < required_extensions[i].major &&
            (major == required_extensions[i].major && minor < required_extensions[i].minor) &&
            !strstr(extensions, required_extensions[i].extension)) {
            av_log(opengl, AV_LOG_ERROR, "Required extension %s is not supported.\n",
                   required_extensions[i].extension);
            av_log(opengl, AV_LOG_DEBUG, "Supported extensions are: %s\n", extensions);
            return AVERROR(ENOSYS);
        }
    }
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &opengl->max_texture_size);
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &opengl->max_viewport_width);
    opengl->non_pow_2_textures = major >= 2 || strstr(extensions, "GL_ARB_texture_non_power_of_two");
#if defined(GL_ES_VERSION_2_0)
    opengl->unpack_subimage = !!strstr(extensions, "GL_EXT_unpack_subimage");
#else
    opengl->unpack_subimage = 1;
#endif
    av_log(opengl, AV_LOG_DEBUG, "Non Power of 2 textures support: %s\n", opengl->non_pow_2_textures ? "Yes" : "No");
    av_log(opengl, AV_LOG_DEBUG, "Unpack Subimage extension support: %s\n", opengl->unpack_subimage ? "Yes" : "No");
    av_log(opengl, AV_LOG_DEBUG, "Max texture size: %dx%d\n", opengl->max_texture_size, opengl->max_texture_size);
    av_log(opengl, AV_LOG_DEBUG, "Max viewport size: %dx%d\n",
           opengl->max_viewport_width, opengl->max_viewport_height);
    OPENGL_ERROR_CHECK(opengl);
    return 0;
  fail:
    return AVERROR_EXTERNAL;
}
