static int xv_write_header(AVFormatContext *s)
{
    XVContext *xv = s->priv_data;
    unsigned int num_adaptors;
    XvAdaptorInfo *ai;
    XvImageFormatValues *fv;
    XColor fgcolor;
    XWindowAttributes window_attrs;
    int num_formats = 0, j, tag, ret;
    AVCodecContext *encctx = s->streams[0]->codec;
    if (   s->nb_streams > 1
        || encctx->codec_type != AVMEDIA_TYPE_VIDEO
        || encctx->codec_id   != AV_CODEC_ID_RAWVIDEO) {
        av_log(s, AV_LOG_ERROR, "Only supports one rawvideo stream\n");
        return AVERROR(EINVAL);
    }
    if (!(tag = xv_get_tag_from_format(encctx->pix_fmt))) {
        av_log(s, AV_LOG_ERROR,
               "Unsupported pixel format '%s', only yuv420p, uyvy422, yuyv422 are currently supported\n",
               av_get_pix_fmt_name(encctx->pix_fmt));
        return AVERROR_PATCHWELCOME;
    }
    xv->image_format = encctx->pix_fmt;
    xv->display = XOpenDisplay(xv->display_name);
    if (!xv->display) {
        av_log(s, AV_LOG_ERROR, "Could not open the X11 display '%s'\n", xv->display_name);
        return AVERROR(EINVAL);
    }
    xv->image_width  = encctx->width;
    xv->image_height = encctx->height;
    if (!xv->window_width && !xv->window_height) {
        AVRational sar = encctx->sample_aspect_ratio;
        xv->window_width  = encctx->width;
        xv->window_height = encctx->height;
        if (sar.num) {
            if (sar.num > sar.den)
                xv->window_width = av_rescale(xv->window_width, sar.num, sar.den);
            if (sar.num < sar.den)
                xv->window_height = av_rescale(xv->window_height, sar.den, sar.num);
        }
    }
    if (!xv->window_id) {
        xv->window = XCreateSimpleWindow(xv->display, DefaultRootWindow(xv->display),
                                         xv->window_x, xv->window_y,
                                         xv->window_width, xv->window_height,
                                         0, 0, 0);
        if (!xv->window_title) {
            if (!(xv->window_title = av_strdup(s->filename))) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
        }
        XStoreName(xv->display, xv->window, xv->window_title);
        xv->wm_delete_message = XInternAtom(xv->display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(xv->display, xv->window, &xv->wm_delete_message, 1);
        XMapWindow(xv->display, xv->window);
    } else
        xv->window = xv->window_id;
    if (XvQueryAdaptors(xv->display, DefaultRootWindow(xv->display), &num_adaptors, &ai) != Success) {
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    if (!num_adaptors) {
        av_log(s, AV_LOG_ERROR, "No X-Video adaptors present\n");
        return AVERROR(ENODEV);
    }
    xv->xv_port = ai[0].base_id;
    XvFreeAdaptorInfo(ai);
    fv = XvListImageFormats(xv->display, xv->xv_port, &num_formats);
    if (!fv) {
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    for (j = 0; j < num_formats; j++) {
        if (fv[j].id == tag) {
            break;
        }
    }
    XFree(fv);
    if (j >= num_formats) {
        av_log(s, AV_LOG_ERROR,
               "Device does not support pixel format %s, aborting\n",
               av_get_pix_fmt_name(encctx->pix_fmt));
        ret = AVERROR(EINVAL);
        goto fail;
    }
    xv->gc = XCreateGC(xv->display, xv->window, 0, 0);
    xv->image_width  = encctx->width;
    xv->image_height = encctx->height;
    xv->yuv_image = XvShmCreateImage(xv->display, xv->xv_port, tag, 0,
                                     xv->image_width, xv->image_height, &xv->yuv_shminfo);
    xv->yuv_shminfo.shmid = shmget(IPC_PRIVATE, xv->yuv_image->data_size,
                                   IPC_CREAT | 0777);
    xv->yuv_shminfo.shmaddr = (char *)shmat(xv->yuv_shminfo.shmid, 0, 0);
    xv->yuv_image->data = xv->yuv_shminfo.shmaddr;
    xv->yuv_shminfo.readOnly = False;
    XShmAttach(xv->display, &xv->yuv_shminfo);
    XSync(xv->display, False);
    shmctl(xv->yuv_shminfo.shmid, IPC_RMID, 0);
    XGetWindowAttributes(xv->display, xv->window, &window_attrs);
    fgcolor.red = fgcolor.green = fgcolor.blue = 0;
    fgcolor.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(xv->display, window_attrs.colormap, &fgcolor);
    XSetForeground(xv->display, xv->gc, fgcolor.pixel);
    
    xv->window_width = xv->window_height = 0;
    return 0;
  fail:
    xv_write_trailer(s);
    return ret;
}
