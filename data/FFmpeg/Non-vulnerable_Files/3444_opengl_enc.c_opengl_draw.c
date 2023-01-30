static int opengl_draw(AVFormatContext *h, void *input, int repaint, int is_pkt)
{
    OpenGLContext *opengl = h->priv_data;
    enum AVPixelFormat pix_fmt = h->streams[0]->codec->pix_fmt;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmt);
    int ret;
#if HAVE_SDL
    if (!opengl->no_window && (ret = opengl_sdl_process_events(h)) < 0)
        goto fail;
#endif
    if (opengl->no_window &&
        (ret = avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_PREPARE_WINDOW_BUFFER, NULL , 0)) < 0) {
        av_log(opengl, AV_LOG_ERROR, "Application failed to prepare window buffer.\n");
        goto fail;
    }
    glClear(GL_COLOR_BUFFER_BIT);
    if (!repaint) {
        if (is_pkt)
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        LOAD_TEXTURE_DATA(0, 0)
        if (desc->flags & AV_PIX_FMT_FLAG_PLANAR) {
            LOAD_TEXTURE_DATA(1, 1)
            LOAD_TEXTURE_DATA(2, 1)
            if (desc->flags & AV_PIX_FMT_FLAG_ALPHA)
                LOAD_TEXTURE_DATA(3, 0)
        }
    }
    ret = AVERROR_EXTERNAL;
    OPENGL_ERROR_CHECK(opengl);
    if ((ret = opengl_prepare(opengl)) < 0)
        goto fail;
    opengl->glprocs.glBindBuffer(FF_GL_ARRAY_BUFFER, opengl->vertex_buffer);
    opengl->glprocs.glBindBuffer(FF_GL_ELEMENT_ARRAY_BUFFER, opengl->index_buffer);
    opengl->glprocs.glVertexAttribPointer(opengl->position_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(OpenGLVertexInfo), 0);
    opengl->glprocs.glEnableVertexAttribArray(opengl->position_attrib);
    opengl->glprocs.glVertexAttribPointer(opengl->texture_coords_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(OpenGLVertexInfo), 12);
    opengl->glprocs.glEnableVertexAttribArray(opengl->texture_coords_attrib);
    glDrawElements(GL_TRIANGLES, FF_ARRAY_ELEMS(g_index), GL_UNSIGNED_SHORT, 0);
    ret = AVERROR_EXTERNAL;
    OPENGL_ERROR_CHECK(opengl);
#if HAVE_SDL
    if (!opengl->no_window)
        SDL_GL_SwapBuffers();
#endif
    if (opengl->no_window &&
        (ret = avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_DISPLAY_WINDOW_BUFFER, NULL , 0)) < 0) {
        av_log(opengl, AV_LOG_ERROR, "Application failed to display window buffer.\n");
        goto fail;
    }
    return 0;
  fail:
    return ret;
}
