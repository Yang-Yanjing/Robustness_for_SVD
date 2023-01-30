static av_cold int opengl_init_context(OpenGLContext *opengl);
static av_cold void opengl_deinit_context(OpenGLContext *opengl)
{
    glDeleteTextures(4, opengl->texture_name);
    opengl->texture_name[0] = opengl->texture_name[1] =
    opengl->texture_name[2] = opengl->texture_name[3] = 0;
    if (opengl->glprocs.glUseProgram)
        opengl->glprocs.glUseProgram(0);
    if (opengl->glprocs.glDeleteProgram) {
        opengl->glprocs.glDeleteProgram(opengl->program);
        opengl->program = 0;
    }
    if (opengl->glprocs.glDeleteShader) {
        opengl->glprocs.glDeleteShader(opengl->vertex_shader);
        opengl->glprocs.glDeleteShader(opengl->fragment_shader);
        opengl->vertex_shader = opengl->fragment_shader = 0;
    }
    if (opengl->glprocs.glBindBuffer) {
        opengl->glprocs.glBindBuffer(FF_GL_ARRAY_BUFFER, 0);
        opengl->glprocs.glBindBuffer(FF_GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    if (opengl->glprocs.glDeleteBuffers) {
        opengl->glprocs.glDeleteBuffers(2, &opengl->index_buffer);
        opengl->vertex_buffer = opengl->index_buffer = 0;
    }
}
