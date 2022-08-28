static int xv_repaint(AVFormatContext *s)
{
    XVContext *xv = s->priv_data;
    XWindowAttributes window_attrs;
    XGetWindowAttributes(xv->display, xv->window, &window_attrs);
    if (window_attrs.width != xv->window_width || window_attrs.height != xv->window_height) {
        XRectangle rect[2];
        xv->dest_w = window_attrs.width;
        xv->dest_h = window_attrs.height;
        compute_display_area(s);
        if (xv->dest_x) {
            rect[0].width  = rect[1].width  = xv->dest_x;
            rect[0].height = rect[1].height = window_attrs.height;
            rect[0].y      = rect[1].y      = 0;
            rect[0].x = 0;
            rect[1].x = xv->dest_w + xv->dest_x;
            XFillRectangles(xv->display, xv->window, xv->gc, rect, 2);
        }
        if (xv->dest_y) {
            rect[0].width  = rect[1].width  = window_attrs.width;
            rect[0].height = rect[1].height = xv->dest_y;
            rect[0].x      = rect[1].x      = 0;
            rect[0].y = 0;
            rect[1].y = xv->dest_h + xv->dest_y;
            XFillRectangles(xv->display, xv->window, xv->gc, rect, 2);
        }
    }
    if (XvShmPutImage(xv->display, xv->xv_port, xv->window, xv->gc,
                      xv->yuv_image, 0, 0, xv->image_width, xv->image_height,
                      xv->dest_x, xv->dest_y, xv->dest_w, xv->dest_h, True) != Success) {
        av_log(s, AV_LOG_ERROR, "Could not copy image to XV shared memory buffer\n");
        return AVERROR_EXTERNAL;
    }
    return 0;
}
