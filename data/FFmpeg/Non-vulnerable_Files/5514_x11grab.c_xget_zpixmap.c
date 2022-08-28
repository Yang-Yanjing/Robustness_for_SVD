static int xget_zpixmap(Display *dpy, Drawable d, XImage *image, int x, int y)
{
    xGetImageReply rep;
    xGetImageReq *req;
    long nbytes;
    if (!image)
        return 0;
    LockDisplay(dpy);
    GetReq(GetImage, req);
    
    req->drawable  = d;
    req->x         = x;
    req->y         = y;
    req->width     = image->width;
    req->height    = image->height;
    req->planeMask = (unsigned int)AllPlanes;
    req->format    = ZPixmap;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse) || !rep.length) {
        UnlockDisplay(dpy);
        SyncHandle();
        return 0;
    }
    nbytes = (long)rep.length << 2;
    _XReadPad(dpy, image->data, nbytes);
    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}
