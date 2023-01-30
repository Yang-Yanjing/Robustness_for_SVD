static void x11grab_draw_region_win(X11GrabContext *s)
{
    Display *dpy = s->dpy;
    Window win   = s->region_win;
    int screen = DefaultScreen(dpy);
    GC gc = XCreateGC(dpy, win, 0, 0);
    XSetForeground(dpy, gc, WhitePixel(dpy, screen));
    XSetBackground(dpy, gc, BlackPixel(dpy, screen));
    XSetLineAttributes(dpy, gc, REGION_WIN_BORDER, LineDoubleDash, 0, 0);
    XDrawRectangle(dpy, win, gc, 1, 1,
                   (s->width  + REGION_WIN_BORDER * 2) - 1 * 2 - 1,
                   (s->height + REGION_WIN_BORDER * 2) - 1 * 2 - 1);
    XFreeGC(dpy, gc);
}
