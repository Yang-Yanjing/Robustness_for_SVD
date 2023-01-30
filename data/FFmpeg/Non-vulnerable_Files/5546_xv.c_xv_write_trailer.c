static int xv_write_trailer(AVFormatContext *s)
{
    XVContext *xv = s->priv_data;
    if (xv->display) {
        XShmDetach(xv->display, &xv->yuv_shminfo);
        if (xv->yuv_image)
            shmdt(xv->yuv_image->data);
        XFree(xv->yuv_image);
        if (xv->gc)
            XFreeGC(xv->display, xv->gc);
        XCloseDisplay(xv->display);
    }
    return 0;
}
