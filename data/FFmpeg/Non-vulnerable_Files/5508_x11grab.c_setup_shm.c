static int setup_shm(AVFormatContext *s, Display *dpy, XImage **image)
{
    X11GrabContext *g = s->priv_data;
    int scr           = XDefaultScreen(dpy);
    XImage *img       = XShmCreateImage(dpy, DefaultVisual(dpy, scr),
                                        DefaultDepth(dpy, scr), ZPixmap, NULL,
                                        &g->shminfo, g->width, g->height);
    g->shminfo.shmid = shmget(IPC_PRIVATE, img->bytes_per_line * img->height,
                              IPC_CREAT | 0777);
    if (g->shminfo.shmid == -1) {
        av_log(s, AV_LOG_ERROR, "Cannot get shared memory!\n");
        return AVERROR(ENOMEM);
    }
    g->shminfo.shmaddr  = img->data = shmat(g->shminfo.shmid, 0, 0);
    g->shminfo.readOnly = False;
    if (!XShmAttach(dpy, &g->shminfo)) {
        av_log(s, AV_LOG_ERROR, "Failed to attach shared memory!\n");
        
        return AVERROR(EIO);
    }
    *image = img;
    return 0;
}
