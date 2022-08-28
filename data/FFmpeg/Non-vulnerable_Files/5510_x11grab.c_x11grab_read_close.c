static int x11grab_read_close(AVFormatContext *s1)
{
    X11GrabContext *x11grab = s1->priv_data;
    
    if (x11grab->use_shm) {
        XShmDetach(x11grab->dpy, &x11grab->shminfo);
        shmdt(x11grab->shminfo.shmaddr);
        shmctl(x11grab->shminfo.shmid, IPC_RMID, NULL);
    }
    
    if (x11grab->image) {
        XDestroyImage(x11grab->image);
        x11grab->image = NULL;
    }
    if (x11grab->region_win)
        XDestroyWindow(x11grab->dpy, x11grab->region_win);
    
    XCloseDisplay(x11grab->dpy);
    return 0;
}
