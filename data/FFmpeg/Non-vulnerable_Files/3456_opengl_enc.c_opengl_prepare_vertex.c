static av_cold int opengl_prepare_vertex(AVFormatContext *s)
{
    OpenGLContext *opengl = s->priv_data;
    int tex_w, tex_h;
    if (opengl->window_width > opengl->max_viewport_width || opengl->window_height > opengl->max_viewport_height) {
        opengl->window_width = FFMIN(opengl->window_width, opengl->max_viewport_width);
        opengl->window_height = FFMIN(opengl->window_height, opengl->max_viewport_height);
        av_log(opengl, AV_LOG_WARNING, "Too big viewport requested, limited to %dx%d", opengl->window_width, opengl->window_height);
    }
    glViewport(0, 0, opengl->window_width, opengl->window_height);
    opengl_make_ortho(opengl->projection_matrix,
                      - (float)opengl->window_width  / 2.0f, (float)opengl->window_width  / 2.0f,
                      - (float)opengl->window_height / 2.0f, (float)opengl->window_height / 2.0f,
                      1.0f, -1.0f);
    opengl_make_identity(opengl->model_view_matrix);
    opengl_compute_display_area(s);
    opengl->vertex[0].z = opengl->vertex[1].z = opengl->vertex[2].z = opengl->vertex[3].z = 0.0f;
    opengl->vertex[0].x = opengl->vertex[1].x = - (float)opengl->picture_width / 2.0f;
    opengl->vertex[2].x = opengl->vertex[3].x =   (float)opengl->picture_width / 2.0f;
    opengl->vertex[1].y = opengl->vertex[2].y = - (float)opengl->picture_height / 2.0f;
    opengl->vertex[0].y = opengl->vertex[3].y =   (float)opengl->picture_height / 2.0f;
    opengl_get_texture_size(opengl, opengl->width, opengl->height, &tex_w, &tex_h);
    opengl->vertex[0].s0 = 0.0f;
    opengl->vertex[0].t0 = 0.0f;
    opengl->vertex[1].s0 = 0.0f;
    opengl->vertex[1].t0 = (float)opengl->height / (float)tex_h;
    opengl->vertex[2].s0 = (float)opengl->width  / (float)tex_w;
    opengl->vertex[2].t0 = (float)opengl->height / (float)tex_h;
    opengl->vertex[3].s0 = (float)opengl->width  / (float)tex_w;
    opengl->vertex[3].t0 = 0.0f;
    opengl->glprocs.glBindBuffer(FF_GL_ARRAY_BUFFER, opengl->vertex_buffer);
    opengl->glprocs.glBufferData(FF_GL_ARRAY_BUFFER, sizeof(opengl->vertex), opengl->vertex, FF_GL_STATIC_DRAW);
    opengl->glprocs.glBindBuffer(FF_GL_ARRAY_BUFFER, 0);
    OPENGL_ERROR_CHECK(opengl);
    return 0;
  fail:
    return AVERROR_EXTERNAL;
}
