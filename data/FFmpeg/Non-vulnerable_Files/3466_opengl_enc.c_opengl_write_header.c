static av_cold int opengl_write_header(AVFormatContext *h)
{
    OpenGLContext *opengl = h->priv_data;
    AVStream *st;
    int ret;
    if (h->nb_streams != 1 ||
        h->streams[0]->codec->codec_type != AVMEDIA_TYPE_VIDEO ||
        h->streams[0]->codec->codec_id != AV_CODEC_ID_RAWVIDEO) {
        av_log(opengl, AV_LOG_ERROR, "Only a single video stream is supported.\n");
        return AVERROR(EINVAL);
    }
    st = h->streams[0];
    opengl->width = st->codec->width;
    opengl->height = st->codec->height;
    opengl->pix_fmt = st->codec->pix_fmt;
    if (!opengl->window_width)
        opengl->window_width = opengl->width;
    if (!opengl->window_height)
        opengl->window_height = opengl->height;
    if (!opengl->window_title && !opengl->no_window)
        opengl->window_title = av_strdup(h->filename);
    if ((ret = opengl_create_window(h)))
        goto fail;
    if ((ret = opengl_read_limits(opengl)) < 0)
        goto fail;
    if (opengl->width > opengl->max_texture_size || opengl->height > opengl->max_texture_size) {
        av_log(opengl, AV_LOG_ERROR, "Too big picture %dx%d, max supported size is %dx%d\n",
               opengl->width, opengl->height, opengl->max_texture_size, opengl->max_texture_size);
        ret = AVERROR(EINVAL);
        goto fail;
    }
    if ((ret = opengl_load_procedures(opengl)) < 0)
        goto fail;
    opengl_fill_color_map(opengl);
    opengl_get_texture_params(opengl);
    if ((ret = opengl_init_context(opengl)) < 0)
        goto fail;
    if ((ret = opengl_prepare_vertex(h)) < 0)
        goto fail;
    glClear(GL_COLOR_BUFFER_BIT);
#if HAVE_SDL
    if (!opengl->no_window)
        SDL_GL_SwapBuffers();
#endif
    if (opengl->no_window &&
        (ret = avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_DISPLAY_WINDOW_BUFFER, NULL , 0)) < 0) {
        av_log(opengl, AV_LOG_ERROR, "Application failed to display window buffer.\n");
        goto fail;
    }
    ret = AVERROR_EXTERNAL;
    OPENGL_ERROR_CHECK(opengl);
    opengl->inited = 1;
    return 0;
  fail:
    opengl_write_trailer(h);
    return ret;
}
