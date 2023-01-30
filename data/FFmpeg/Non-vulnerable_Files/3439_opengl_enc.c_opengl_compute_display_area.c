static void opengl_compute_display_area(AVFormatContext *s)
{
    AVRational sar, dar; 
    OpenGLContext *opengl = s->priv_data;
    AVStream *st = s->streams[0];
    AVCodecContext *encctx = st->codec;
    
    sar = st->sample_aspect_ratio.num ? st->sample_aspect_ratio : (AVRational){ 1, 1 };
    dar = av_mul_q(sar, (AVRational){ encctx->width, encctx->height });
    
    
    if (av_cmp_q(dar, (AVRational){ opengl->window_width, opengl->window_height }) > 0) {
        
        opengl->picture_width = opengl->window_width;
        opengl->picture_height = av_rescale(opengl->picture_width, dar.den, dar.num);
    } else {
        
        opengl->picture_height = opengl->window_height;
        opengl->picture_width = av_rescale(opengl->picture_height, dar.num, dar.den);
    }
