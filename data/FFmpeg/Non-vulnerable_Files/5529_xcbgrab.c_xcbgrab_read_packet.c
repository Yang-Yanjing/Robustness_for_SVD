static int xcbgrab_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    XCBGrabContext *c = s->priv_data;
    xcb_query_pointer_cookie_t pc;
    xcb_get_geometry_cookie_t gc;
    xcb_query_pointer_reply_t *p  = NULL;
    xcb_get_geometry_reply_t *geo = NULL;
    int ret = 0;
    wait_frame(s, pkt);
    if (c->follow_mouse || c->draw_mouse) {
        pc  = xcb_query_pointer(c->conn, c->screen->root);
        gc  = xcb_get_geometry(c->conn, c->screen->root);
        p   = xcb_query_pointer_reply(c->conn, pc, NULL);
        geo = xcb_get_geometry_reply(c->conn, gc, NULL);
    }
    if (c->follow_mouse && p->same_screen)
        xcbgrab_reposition(s, p, geo);
    if (c->show_region)
        xcbgrab_update_region(s);
#if CONFIG_LIBXCB_SHM
    if (c->has_shm && xcbgrab_frame_shm(s, pkt) < 0)
        c->has_shm = 0;
#endif
    if (!c->has_shm)
        ret = xcbgrab_frame(s, pkt);
#if CONFIG_LIBXCB_XFIXES
    if (ret >= 0 && c->draw_mouse && p->same_screen)
        xcbgrab_draw_mouse(s, pkt, p, geo);
#endif
    free(p);
    free(geo);
    return ret;
}
