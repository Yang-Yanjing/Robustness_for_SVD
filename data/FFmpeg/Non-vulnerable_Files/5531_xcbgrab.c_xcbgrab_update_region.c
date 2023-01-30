static void xcbgrab_update_region(AVFormatContext *s)
{
    XCBGrabContext *c     = s->priv_data;
    const uint32_t args[] = { c->x - c->region_border,
                              c->y - c->region_border };
    xcb_configure_window(c->conn,
                         c->window,
                         XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
                         args);
}
