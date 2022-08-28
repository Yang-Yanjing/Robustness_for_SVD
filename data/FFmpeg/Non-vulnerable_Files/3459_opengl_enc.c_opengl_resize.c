static int opengl_resize(AVFormatContext *h, int width, int height)
{
    int ret = 0;
    OpenGLContext *opengl = h->priv_data;
    opengl->window_width = width;
    opengl->window_height = height;
    if (opengl->inited) {
        if (opengl->no_window &&
            (ret = avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_PREPARE_WINDOW_BUFFER, NULL , 0)) < 0) {
            av_log(opengl, AV_LOG_ERROR, "Application failed to prepare window buffer.\n");
            goto end;
        }
        if ((ret = opengl_prepare_vertex(h)) < 0)
            goto end;
        ret = opengl_draw(h, NULL, 1, 0);
    }
  end:
    return ret;
}
