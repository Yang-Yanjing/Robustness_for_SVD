static int opengl_prepare(OpenGLContext *opengl)
{
    int i;
    opengl->glprocs.glUseProgram(opengl->program);
    opengl->glprocs.glUniformMatrix4fv(opengl->projection_matrix_location, 1, GL_FALSE, opengl->projection_matrix);
    opengl->glprocs.glUniformMatrix4fv(opengl->model_view_matrix_location, 1, GL_FALSE, opengl->model_view_matrix);
    for (i = 0; i < 4; i++)
        if (opengl->texture_location[i] != -1) {
            opengl->glprocs.glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, opengl->texture_name[i]);
            opengl->glprocs.glUniform1i(opengl->texture_location[i], i);
        }
    if (opengl->color_map_location != -1)
        opengl->glprocs.glUniformMatrix4fv(opengl->color_map_location, 1, GL_FALSE, opengl->color_map);
    if (opengl->chroma_div_h_location != -1)
        opengl->glprocs.glUniform1f(opengl->chroma_div_h_location, opengl->chroma_div_h);
    if (opengl->chroma_div_w_location != -1)
        opengl->glprocs.glUniform1f(opengl->chroma_div_w_location, opengl->chroma_div_w);
    OPENGL_ERROR_CHECK(opengl);
    return 0;
  fail:
    return AVERROR_EXTERNAL;
}
