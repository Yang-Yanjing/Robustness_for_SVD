static av_cold GLuint opengl_load_shader(OpenGLContext *opengl, GLenum type, const char *source)
{
    GLuint shader = opengl->glprocs.glCreateShader(type);
    GLint result;
    if (!shader) {
        av_log(opengl, AV_LOG_ERROR, "glCreateShader() failed\n");
        return 0;
    }
    opengl->glprocs.glShaderSource(shader, 1, &source, NULL);
    opengl->glprocs.glCompileShader(shader);
    opengl->glprocs.glGetShaderiv(shader, FF_GL_COMPILE_STATUS, &result);
    if (!result) {
        char *log;
        opengl->glprocs.glGetShaderiv(shader, FF_GL_INFO_LOG_LENGTH, &result);
        if (result) {
            if ((log = av_malloc(result))) {
                opengl->glprocs.glGetShaderInfoLog(shader, result, NULL, log);
                av_log(opengl, AV_LOG_ERROR, "Compile error: %s\n", log);
                av_free(log);
            }
        }
        goto fail;
    }
    OPENGL_ERROR_CHECK(opengl);
    return shader;
  fail:
    opengl->glprocs.glDeleteShader(shader);
    return 0;
}
