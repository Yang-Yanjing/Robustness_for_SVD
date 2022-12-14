static av_cold int fbdev_read_header(AVFormatContext *avctx)
{
    FBDevContext *fbdev = avctx->priv_data;
    AVStream *st = NULL;
    enum AVPixelFormat pix_fmt;
    int ret, flags = O_RDONLY;
    const char* device;
    if (!(st = avformat_new_stream(avctx, NULL)))
        return AVERROR(ENOMEM);
    avpriv_set_pts_info(st, 64, 1, 1000000); 
    
    if (avctx->flags & AVFMT_FLAG_NONBLOCK)
        flags |= O_NONBLOCK;
    if (avctx->filename[0])
        device = avctx->filename;
    else
        device = ff_fbdev_default_device();
    if ((fbdev->fd = avpriv_open(device, flags)) == -1) {
        ret = AVERROR(errno);
        av_log(avctx, AV_LOG_ERROR,
               "Could not open framebuffer device '%s': %s\n",
               device, av_err2str(ret));
        return ret;
    }
    if (ioctl(fbdev->fd, FBIOGET_VSCREENINFO, &fbdev->varinfo) < 0) {
        ret = AVERROR(errno);
        av_log(avctx, AV_LOG_ERROR,
               "FBIOGET_VSCREENINFO: %s\n", av_err2str(ret));
        goto fail;
    }
    if (ioctl(fbdev->fd, FBIOGET_FSCREENINFO, &fbdev->fixinfo) < 0) {
        ret = AVERROR(errno);
        av_log(avctx, AV_LOG_ERROR,
               "FBIOGET_FSCREENINFO: %s\n", av_err2str(ret));
        goto fail;
    }
    pix_fmt = ff_get_pixfmt_from_fb_varinfo(&fbdev->varinfo);
    if (pix_fmt == AV_PIX_FMT_NONE) {
        ret = AVERROR(EINVAL);
        av_log(avctx, AV_LOG_ERROR,
               "Framebuffer pixel format not supported.\n");
        goto fail;
    }
    fbdev->width           = fbdev->varinfo.xres;
    fbdev->height          = fbdev->varinfo.yres;
    fbdev->bytes_per_pixel = (fbdev->varinfo.bits_per_pixel + 7) >> 3;
    fbdev->frame_linesize  = fbdev->width * fbdev->bytes_per_pixel;
    fbdev->frame_size      = fbdev->frame_linesize * fbdev->height;
    fbdev->time_frame      = AV_NOPTS_VALUE;
    fbdev->data = mmap(NULL, fbdev->fixinfo.smem_len, PROT_READ, MAP_SHARED, fbdev->fd, 0);
    if (fbdev->data == MAP_FAILED) {
        ret = AVERROR(errno);
        av_log(avctx, AV_LOG_ERROR, "Error in mmap(): %s\n", av_err2str(ret));
        goto fail;
    }
    st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    st->codec->codec_id   = AV_CODEC_ID_RAWVIDEO;
    st->codec->width      = fbdev->width;
    st->codec->height     = fbdev->height;
    st->codec->pix_fmt    = pix_fmt;
    st->codec->time_base  = av_inv_q(fbdev->framerate_q);
    st->codec->bit_rate   =
        fbdev->width * fbdev->height * fbdev->bytes_per_pixel * av_q2d(fbdev->framerate_q) * 8;
    av_log(avctx, AV_LOG_INFO,
           "w:%d h:%d bpp:%d pixfmt:%s fps:%d/%d bit_rate:%d\n",
           fbdev->width, fbdev->height, fbdev->varinfo.bits_per_pixel,
           av_get_pix_fmt_name(pix_fmt),
           fbdev->framerate_q.num, fbdev->framerate_q.den,
           st->codec->bit_rate);
    return 0;
fail:
    close(fbdev->fd);
    return ret;
}
