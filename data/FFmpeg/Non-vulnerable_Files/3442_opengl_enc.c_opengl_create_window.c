static int opengl_create_window(AVFormatContext *h)
{
    OpenGLContext *opengl = h->priv_data;
    int ret;
    if (!opengl->no_window) {
#if HAVE_SDL
        if ((ret = opengl_sdl_create_window(h)) < 0) {
            av_log(opengl, AV_LOG_ERROR, "Cannot create default SDL window.\n");
            return ret;
        }
#else
        av_log(opengl, AV_LOG_ERROR, "FFmpeg is compiled without SDL. Cannot create default window.\n");
        return AVERROR(ENOSYS);
#endif
    } else {
        AVDeviceRect message;
        message.x = message.y = 0;
        message.width = opengl->window_width;
        message.height = opengl->window_height;
        if ((ret = avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_CREATE_WINDOW_BUFFER,
                                                       &message , sizeof(message))) < 0) {
            av_log(opengl, AV_LOG_ERROR, "Application failed to create window buffer.\n");
            return ret;
        }
        if ((ret = avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_PREPARE_WINDOW_BUFFER, NULL , 0)) < 0) {
            av_log(opengl, AV_LOG_ERROR, "Application failed to prepare window buffer.\n");
            return ret;
        }
    }
    return 0;
}
