static int opengl_release_window(AVFormatContext *h)
{
    int ret;
    OpenGLContext *opengl = h->priv_data;
    if (!opengl->no_window) {
#if HAVE_SDL
        SDL_Quit();
#endif
    } else if ((ret = avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_DESTROY_WINDOW_BUFFER, NULL , 0)) < 0) {
        av_log(opengl, AV_LOG_ERROR, "Application failed to release window buffer.\n");
        return ret;
    }
    return 0;
}
