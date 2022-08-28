static void x11grab_region_win_init(X11GrabContext *s)
{
    Display *dpy = s->dpy;
    XRectangle rect;
    XSetWindowAttributes attribs = { .override_redirect = True };
    int screen = DefaultScreen(dpy);
    s->region_win = XCreateWindow(dpy, RootWindow(dpy, screen),
                                  s->x_off  - REGION_WIN_BORDER,
                                  s->y_off  - REGION_WIN_BORDER,
                                  s->width  + REGION_WIN_BORDER * 2,
                                  s->height + REGION_WIN_BORDER * 2,
                                  0, CopyFromParent,
                                  InputOutput, CopyFromParent,
                                  CWOverrideRedirect, &attribs);
    rect.x      = 0;
    rect.y      = 0;
    rect.width  = s->width;
    rect.height = s->height;
    XShapeCombineRectangles(dpy, s->region_win,
                            ShapeBounding, REGION_WIN_BORDER, REGION_WIN_BORDER,
                            &rect, 1, ShapeSubtract, 0);
    XMapWindow(dpy, s->region_win);
    XSelectInput(dpy, s->region_win, ExposureMask | StructureNotifyMask);
    x11grab_draw_region_win(s);
}
