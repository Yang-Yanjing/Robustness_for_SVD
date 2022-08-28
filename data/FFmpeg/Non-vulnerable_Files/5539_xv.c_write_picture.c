static int write_picture(AVFormatContext *s, AVPicture *pict)
{
    XVContext *xv = s->priv_data;
    XvImage *img = xv->yuv_image;
    uint8_t *data[3] = {
        img->data + img->offsets[0],
        img->data + img->offsets[1],
        img->data + img->offsets[2]
    };
    
    if (!xv->window_id) {
        XEvent event;
        while (XPending(xv->display)) {
            XNextEvent(xv->display, &event);
            if (event.type == ClientMessage && event.xclient.data.l[0] == xv->wm_delete_message) {
                av_log(xv, AV_LOG_DEBUG, "Window close event.\n");
                return AVERROR(EPIPE);
            }
        }
    }
    av_image_copy(data, img->pitches, (const uint8_t **)pict->data, pict->linesize,
                  xv->image_format, img->width, img->height);
    return xv_repaint(s);
}
