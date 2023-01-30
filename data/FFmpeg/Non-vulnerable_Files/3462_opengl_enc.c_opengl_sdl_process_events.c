static int opengl_sdl_process_events(AVFormatContext *h)
{
    int ret;
    OpenGLContext *opengl = h->priv_data;
    SDL_Event event;
    SDL_PumpEvents();
    while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_ALLEVENTS) > 0) {
        switch (event.type) {
        case SDL_QUIT:
            return AVERROR(EIO);
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
            case SDLK_q:
                return AVERROR(EIO);
            }
            return 0;
        case SDL_VIDEORESIZE: {
            char buffer[100];
            int reinit;
            AVDeviceRect message;
            
            SDL_VideoDriverName(buffer, sizeof(buffer));
            reinit = !av_strncasecmp(buffer, "quartz", sizeof(buffer));
            if (reinit) {
                opengl_deinit_context(opengl);
            }
            if ((ret = opengl_sdl_recreate_window(opengl, event.resize.w, event.resize.h)) < 0)
                return ret;
            if (reinit && (ret = opengl_init_context(opengl)) < 0)
                return ret;
            message.width = opengl->surface->w;
            message.height = opengl->surface->h;
            return opengl_control_message(h, AV_APP_TO_DEV_WINDOW_SIZE, &message, sizeof(AVDeviceRect));
            }
        }
    }
    return 0;
}
