static int setup_mouse(Display *dpy, int screen)
{
    int ev_ret, ev_err;
    if (XFixesQueryExtension(dpy, &ev_ret, &ev_err)) {
        Window root = RootWindow(dpy, screen);
        XFixesSelectCursorInput(dpy, root, XFixesDisplayCursorNotifyMask);
        return 0;
    }
    return AVERROR(ENOSYS);
}
