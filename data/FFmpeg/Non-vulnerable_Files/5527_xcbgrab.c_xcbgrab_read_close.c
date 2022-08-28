static av_cold int xcbgrab_read_close(AVFormatContext *s)
{
    XCBGrabContext *ctx = s->priv_data;
    xcb_disconnect(ctx->conn);
    return 0;
}
