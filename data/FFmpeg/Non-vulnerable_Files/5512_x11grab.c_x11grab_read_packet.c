static int x11grab_read_packet(AVFormatContext *s1, AVPacket *pkt)
{
    X11GrabContext *s = s1->priv_data;
    Display *dpy      = s->dpy;
    XImage *image     = s->image;
    int x_off         = s->x_off;
    int y_off         = s->y_off;
    int follow_mouse  = s->follow_mouse;
    int screen, pointer_x, pointer_y, _, same_screen = 1;
    Window w, root;
    int64_t curtime, delay;
    struct timespec ts;
    
    s->time_frame += INT64_C(1000000);
    
    for (;;) {
        curtime = av_gettime();
        delay   = s->time_frame * av_q2d(s->time_base) - curtime;
        if (delay <= 0) {
            if (delay < INT64_C(-1000000) * av_q2d(s->time_base))
                s->time_frame += INT64_C(1000000);
            break;
        }
        ts.tv_sec  = delay / 1000000;
        ts.tv_nsec = (delay % 1000000) * 1000;
        nanosleep(&ts, NULL);
    }
    av_init_packet(pkt);
    pkt->data = image->data;
    pkt->size = s->frame_size;
    pkt->pts  = curtime;
    if (s->palette_changed) {
        uint8_t *pal = av_packet_new_side_data(pkt, AV_PKT_DATA_PALETTE,
                                               AVPALETTE_SIZE);
        if (!pal) {
            av_log(s, AV_LOG_ERROR, "Cannot append palette to packet\n");
        } else {
            memcpy(pal, s->palette, AVPALETTE_SIZE);
            s->palette_changed = 0;
        }
    }
    screen = DefaultScreen(dpy);
    root   = RootWindow(dpy, screen);
    if (follow_mouse || s->draw_mouse)
        same_screen = XQueryPointer(dpy, root, &w, &w,
                                    &pointer_x, &pointer_y, &_, &_, &_);
    if (follow_mouse && same_screen) {
        int screen_w, screen_h;
        screen_w = DisplayWidth(dpy, screen);
        screen_h = DisplayHeight(dpy, screen);
        if (follow_mouse == -1) {
            
            x_off += pointer_x - s->width / 2 - x_off;
            y_off += pointer_y - s->height / 2 - y_off;
        } else {
            
            
            if (pointer_x > x_off + s->width - follow_mouse)
                x_off += pointer_x - (x_off + s->width - follow_mouse);
            else if (pointer_x < x_off + follow_mouse)
                x_off -= (x_off + follow_mouse) - pointer_x;
            if (pointer_y > y_off + s->height - follow_mouse)
                y_off += pointer_y - (y_off + s->height - follow_mouse);
            else if (pointer_y < y_off + follow_mouse)
                y_off -= (y_off + follow_mouse) - pointer_y;
        }
        
        s->x_off = x_off = av_clip(x_off, 0, screen_w - s->width);
        s->y_off = y_off = av_clip(y_off, 0, screen_h - s->height);
        if (s->show_region && s->region_win)
            XMoveWindow(dpy, s->region_win,
                        s->x_off - REGION_WIN_BORDER,
                        s->y_off - REGION_WIN_BORDER);
    }
    if (s->show_region && same_screen) {
        if (s->region_win) {
            XEvent evt = { .type = NoEventMask };
            
            while (XCheckMaskEvent(dpy, ExposureMask | StructureNotifyMask,
                                   &evt))
                ;
            if (evt.type)
                x11grab_draw_region_win(s);
        } else {
            x11grab_region_win_init(s);
        }
    }
    if (s->use_shm) {
        if (!XShmGetImage(dpy, root, image, x_off, y_off, AllPlanes))
            av_log(s1, AV_LOG_INFO, "XShmGetImage() failed\n");
    } else {
        if (!xget_zpixmap(dpy, root, image, x_off, y_off))
            av_log(s1, AV_LOG_INFO, "XGetZPixmap() failed\n");
    }
    if (s->draw_mouse && same_screen)
        paint_mouse_pointer(image, s1);
    return s->frame_size;
}
