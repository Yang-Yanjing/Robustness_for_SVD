static av_cold int opengl_write_trailer(AVFormatContext *h)
{
    OpenGLContext *opengl = h->priv_data;
    if (opengl->no_window &&
        avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_PREPARE_WINDOW_BUFFER, NULL , 0) < 0)
        av_log(opengl, AV_LOG_ERROR, "Application failed to prepare window buffer.\n");
    opengl_deinit_context(opengl);
    opengl_release_window(h);
    return 0;
}
