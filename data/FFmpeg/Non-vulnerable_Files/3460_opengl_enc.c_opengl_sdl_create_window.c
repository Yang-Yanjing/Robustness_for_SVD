static int av_cold opengl_sdl_create_window(AVFormatContext *h)
{
    int ret;
    char buffer[100];
    OpenGLContext *opengl = h->priv_data;
    AVDeviceRect message;
    if (SDL_Init(SDL_INIT_VIDEO)) {
        av_log(opengl, AV_LOG_ERROR, "Unable to initialize SDL: %s\n", SDL_GetError());
        return AVERROR_EXTERNAL;
    }
    if ((ret = opengl_sdl_recreate_window(opengl, opengl->window_width,
                                          opengl->window_height)) < 0)
        return ret;
    av_log(opengl, AV_LOG_INFO, "SDL driver: '%s'.\n", SDL_VideoDriverName(buffer, sizeof(buffer)));
    message.width = opengl->surface->w;
    message.height = opengl->surface->h;
    SDL_WM_SetCaption(opengl->window_title, NULL);
    opengl_control_message(h, AV_APP_TO_DEV_WINDOW_SIZE, &message, sizeof(AVDeviceRect));
    return 0;
}
