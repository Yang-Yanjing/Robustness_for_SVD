static int sdl_write_header(AVFormatContext *s)
{
    SDLContext *sdl = s->priv_data;
    AVStream *st = s->streams[0];
    AVCodecContext *encctx = st->codec;
    int i, ret;
    if (!sdl->window_title)
        sdl->window_title = av_strdup(s->filename);
    if (!sdl->icon_title)
        sdl->icon_title = av_strdup(sdl->window_title);
    if (SDL_WasInit(SDL_INIT_VIDEO)) {
        av_log(s, AV_LOG_ERROR,
               "SDL video subsystem was already inited, aborting\n");
        sdl->sdl_was_already_inited = 1;
        ret = AVERROR(EINVAL);
        goto fail;
    }
    if (   s->nb_streams > 1
        || encctx->codec_type != AVMEDIA_TYPE_VIDEO
        || encctx->codec_id   != AV_CODEC_ID_RAWVIDEO) {
        av_log(s, AV_LOG_ERROR, "Only supports one rawvideo stream\n");
        ret = AVERROR(EINVAL);
        goto fail;
    }
    for (i = 0; sdl_overlay_pix_fmt_map[i].pix_fmt != AV_PIX_FMT_NONE; i++) {
        if (sdl_overlay_pix_fmt_map[i].pix_fmt == encctx->pix_fmt) {
            sdl->overlay_fmt = sdl_overlay_pix_fmt_map[i].overlay_fmt;
            break;
        }
    }
    if (!sdl->overlay_fmt) {
        av_log(s, AV_LOG_ERROR,
               "Unsupported pixel format '%s', choose one of yuv420p, yuyv422, or uyvy422\n",
               av_get_pix_fmt_name(encctx->pix_fmt));
        ret = AVERROR(EINVAL);
        goto fail;
    }
    
    compute_overlay_rect(s);
    sdl->init_cond = SDL_CreateCond();
    if (!sdl->init_cond) {
        av_log(s, AV_LOG_ERROR, "Could not create SDL condition variable: %s\n", SDL_GetError());
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    sdl->mutex = SDL_CreateMutex();
    if (!sdl->mutex) {
        av_log(s, AV_LOG_ERROR, "Could not create SDL mutex: %s\n", SDL_GetError());
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    sdl->event_thread = SDL_CreateThread(event_thread, s);
    if (!sdl->event_thread) {
        av_log(s, AV_LOG_ERROR, "Could not create SDL event thread: %s\n", SDL_GetError());
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    
    SDL_LockMutex(sdl->mutex);
    while (!sdl->inited) {
        SDL_CondWait(sdl->init_cond, sdl->mutex);
    }
    SDL_UnlockMutex(sdl->mutex);
    if (sdl->init_ret < 0) {
        ret = sdl->init_ret;
        goto fail;
    }
    return 0;
fail:
    sdl_write_trailer(s);
    return ret;
}
