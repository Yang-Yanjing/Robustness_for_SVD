static int sdl_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    SDLContext *sdl = s->priv_data;
    AVCodecContext *encctx = s->streams[0]->codec;
    AVPicture pict;
    int i;
    if (sdl->quit) {
        sdl_write_trailer(s);
        return AVERROR(EIO);
    }
    avpicture_fill(&pict, pkt->data, encctx->pix_fmt, encctx->width, encctx->height);
    SDL_LockMutex(sdl->mutex);
    SDL_FillRect(sdl->surface, &sdl->surface->clip_rect,
                 SDL_MapRGB(sdl->surface->format, 0, 0, 0));
    SDL_LockYUVOverlay(sdl->overlay);
    for (i = 0; i < 3; i++) {
        sdl->overlay->pixels [i] = pict.data    [i];
        sdl->overlay->pitches[i] = pict.linesize[i];
    }
    SDL_DisplayYUVOverlay(sdl->overlay, &sdl->overlay_rect);
    SDL_UnlockYUVOverlay(sdl->overlay);
    SDL_UpdateRect(sdl->surface,
                   sdl->overlay_rect.x, sdl->overlay_rect.y,
                   sdl->overlay_rect.w, sdl->overlay_rect.h);
    SDL_UnlockMutex(sdl->mutex);
    return 0;
}
