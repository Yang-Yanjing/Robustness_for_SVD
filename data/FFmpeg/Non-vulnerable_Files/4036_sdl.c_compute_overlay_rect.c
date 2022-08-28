static void compute_overlay_rect(AVFormatContext *s)
{
    AVRational sar, dar; 
    SDLContext *sdl = s->priv_data;
    AVStream *st = s->streams[0];
    AVCodecContext *encctx = st->codec;
    SDL_Rect *overlay_rect = &sdl->overlay_rect;
    
    sar = st->sample_aspect_ratio.num ? st->sample_aspect_ratio : (AVRational){ 1, 1 };
    dar = av_mul_q(sar, (AVRational){ encctx->width, encctx->height });
    
    if (sdl->window_width && sdl->window_height) {
        
        if (av_cmp_q(dar, (AVRational){ sdl->window_width, sdl->window_height }) > 0) {
            
            overlay_rect->w = sdl->window_width;
            overlay_rect->h = av_rescale(overlay_rect->w, dar.den, dar.num);
        } else {
            
            overlay_rect->h = sdl->window_height;
            overlay_rect->w = av_rescale(overlay_rect->h, dar.num, dar.den);
        }
    } else {
        if (sar.num > sar.den) {
            overlay_rect->w = encctx->width;
            overlay_rect->h = av_rescale(overlay_rect->w, dar.den, dar.num);
        } else {
            overlay_rect->h = encctx->height;
            overlay_rect->w = av_rescale(overlay_rect->h, dar.num, dar.den);
        }
        sdl->window_width  = overlay_rect->w;
        sdl->window_height = overlay_rect->h;
    }
