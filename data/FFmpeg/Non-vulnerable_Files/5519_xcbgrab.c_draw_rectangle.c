static void draw_rectangle(AVFormatContext *s)
{
    XCBGrabContext *c = s->priv_data;
    xcb_gcontext_t gc = xcb_generate_id(c->conn);
    uint32_t mask     = XCB_GC_FOREGROUND |
                        XCB_GC_BACKGROUND |
                        XCB_GC_LINE_WIDTH |
                        XCB_GC_LINE_STYLE |
                        XCB_GC_FILL_STYLE;
    uint32_t values[] = { c->screen->black_pixel,
                          c->screen->white_pixel,
                          c->region_border,
                          XCB_LINE_STYLE_DOUBLE_DASH,
                          XCB_FILL_STYLE_SOLID };
    xcb_rectangle_t r = { 1, 1,
                          c->width  + c->region_border * 2 - 3,
                          c->height + c->region_border * 2 - 3 };
    xcb_create_gc(c->conn, gc, c->window, mask, values);
    xcb_poly_rectangle(c->conn, c->window, gc, 1, &r);
}
