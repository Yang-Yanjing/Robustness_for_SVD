static int xcbgrab_frame(AVFormatContext *s, AVPacket *pkt)
{
    XCBGrabContext *c = s->priv_data;
    xcb_get_image_cookie_t iq;
    xcb_get_image_reply_t *img;
    xcb_drawable_t drawable = c->screen->root;
    xcb_generic_error_t *e = NULL;
    uint8_t *data;
    int length, ret;
    iq  = xcb_get_image(c->conn, XCB_IMAGE_FORMAT_Z_PIXMAP, drawable,
                        c->x, c->y, c->width, c->height, ~0);
    img = xcb_get_image_reply(c->conn, iq, &e);
    if (e) {
        av_log(s, AV_LOG_ERROR,
               "Cannot get the image data "
               "event_error: response_type:%u error_code:%u "
               "sequence:%u resource_id:%u minor_code:%u major_code:%u.\n",
               e->response_type, e->error_code,
               e->sequence, e->resource_id, e->minor_code, e->major_code);
        return AVERROR(EACCES);
    }
    if (!img)
        return AVERROR(EAGAIN);
    data   = xcb_get_image_data(img);
    length = xcb_get_image_data_length(img);
    ret = av_new_packet(pkt, length);
    if (!ret)
        memcpy(pkt->data, data, length);
    free(img);
    return ret;
}
