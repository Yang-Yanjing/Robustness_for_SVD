static void compute_display_area(AVFormatContext *s)
{
    XVContext *xv = s->priv_data;
    AVRational sar, dar; 
    AVStream *st = s->streams[0];
    AVCodecContext *encctx = st->codec;
    
    sar = st->sample_aspect_ratio.num ? st->sample_aspect_ratio : (AVRational){ 1, 1 };
    dar = av_mul_q(sar, (AVRational){ encctx->width, encctx->height });
    
    
    if (av_cmp_q(dar, (AVRational){ xv->dest_w, xv->dest_h }) > 0) {
        
        xv->dest_y = xv->dest_h;
        xv->dest_x = 0;
        xv->dest_h = av_rescale(xv->dest_w, dar.den, dar.num);
        xv->dest_y -= xv->dest_h;
        xv->dest_y /= 2;
    } else {
        
        xv->dest_x = xv->dest_w;
        xv->dest_y = 0;
        xv->dest_w = av_rescale(xv->dest_h, dar.num, dar.den);
        xv->dest_x -= xv->dest_w;
        xv->dest_x /= 2;
    }
