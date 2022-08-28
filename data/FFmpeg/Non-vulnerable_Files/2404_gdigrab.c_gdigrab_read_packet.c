static int gdigrab_read_packet(AVFormatContext *s1, AVPacket *pkt)
{
    struct gdigrab *gdigrab = s1->priv_data;
    HDC        dest_hdc   = gdigrab->dest_hdc;
    HDC        source_hdc = gdigrab->source_hdc;
    RECT       clip_rect  = gdigrab->clip_rect;
    AVRational time_base  = gdigrab->time_base;
    int64_t    time_frame = gdigrab->time_frame;
    BITMAPFILEHEADER bfh;
    int file_size = gdigrab->header_size + gdigrab->frame_size;
    int64_t curtime, delay;
    
    time_frame += INT64_C(1000000);
    
    if (gdigrab->show_region)
        gdigrab_region_wnd_update(s1, gdigrab);
    
    for (;;) {
        curtime = av_gettime();
        delay = time_frame * av_q2d(time_base) - curtime;
        if (delay <= 0) {
            if (delay < INT64_C(-1000000) * av_q2d(time_base)) {
                time_frame += INT64_C(1000000);
            }
            break;
        }
        if (s1->flags & AVFMT_FLAG_NONBLOCK) {
            return AVERROR(EAGAIN);
        } else {
            av_usleep(delay);
        }
    }
    if (av_new_packet(pkt, file_size) < 0)
        return AVERROR(ENOMEM);
    pkt->pts = curtime;
    
    if (!BitBlt(dest_hdc, 0, 0,
                clip_rect.right - clip_rect.left,
                clip_rect.bottom - clip_rect.top,
                source_hdc,
                clip_rect.left, clip_rect.top, SRCCOPY | CAPTUREBLT)) {
        WIN32_API_ERROR("Failed to capture image");
        return AVERROR(EIO);
    }
    if (gdigrab->draw_mouse)
        paint_mouse_pointer(s1, gdigrab);
    
    bfh.bfType = 0x4d42; 
    bfh.bfSize = file_size;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = gdigrab->header_size;
    memcpy(pkt->data, &bfh, sizeof(bfh));
    memcpy(pkt->data + sizeof(bfh), &gdigrab->bmi.bmiHeader, sizeof(gdigrab->bmi.bmiHeader));
    if (gdigrab->bmi.bmiHeader.biBitCount <= 8)
        GetDIBColorTable(dest_hdc, 0, 1 << gdigrab->bmi.bmiHeader.biBitCount,
                (RGBQUAD *) (pkt->data + sizeof(bfh) + sizeof(gdigrab->bmi.bmiHeader)));
    memcpy(pkt->data + gdigrab->header_size, gdigrab->buffer, gdigrab->frame_size);
    gdigrab->time_frame = time_frame;
    return gdigrab->header_size + gdigrab->frame_size;
}
