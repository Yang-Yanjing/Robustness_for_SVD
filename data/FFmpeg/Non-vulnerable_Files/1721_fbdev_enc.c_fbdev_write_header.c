static av_cold int fbdev_write_header(AVFormatContext *h)
{
    FBDevContext *fbdev = h->priv_data;
    enum AVPixelFormat pix_fmt;
    int ret, flags = O_RDWR;
    const char* device;
    if (h->nb_streams != 1 || h->streams[0]->codec->codec_type != AVMEDIA_TYPE_VIDEO) {
        av_log(fbdev, AV_LOG_ERROR, "Only a single video stream is supported.\n");
        return AVERROR(EINVAL);
    }
    if (h->filename[0])
        device = h->filename;
    else
        device = ff_fbdev_default_device();
    if ((fbdev->fd = avpriv_open(device, flags)) == -1) {
        ret = AVERROR(errno);
        av_log(h, AV_LOG_ERROR,
               "Could not open framebuffer device '%s': %s\n",
               device, av_err2str(ret));
        return ret;
    }
    if (ioctl(fbdev->fd, FBIOGET_VSCREENINFO, &fbdev->varinfo) < 0) {
        ret = AVERROR(errno);
        av_log(h, AV_LOG_ERROR, "FBIOGET_VSCREENINFO: %s\n", av_err2str(ret));
        goto fail;
    }
    if (ioctl(fbdev->fd, FBIOGET_FSCREENINFO, &fbdev->fixinfo) < 0) {
        ret = AVERROR(errno);
        av_log(h, AV_LOG_ERROR, "FBIOGET_FSCREENINFO: %s\n", av_err2str(ret));
        goto fail;
    }
    pix_fmt = ff_get_pixfmt_from_fb_varinfo(&fbdev->varinfo);
    if (pix_fmt == AV_PIX_FMT_NONE) {
        ret = AVERROR(EINVAL);
        av_log(h, AV_LOG_ERROR, "Framebuffer pixel format not supported.\n");
        goto fail;
    }
    fbdev->data = mmap(NULL, fbdev->fixinfo.smem_len, PROT_WRITE, MAP_SHARED, fbdev->fd, 0);
    if (fbdev->data == MAP_FAILED) {
        ret = AVERROR(errno);
        av_log(h, AV_LOG_ERROR, "Error in mmap(): %s\n", av_err2str(ret));
        goto fail;
    }
    return 0;
  fail:
    close(fbdev->fd);
    return ret;
}
