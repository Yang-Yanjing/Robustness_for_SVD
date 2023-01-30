static av_cold int write_header(AVFormatContext *s1)
{
    int res = 0, flags = O_RDWR;
    struct v4l2_format fmt = {
        .type = V4L2_BUF_TYPE_VIDEO_OUTPUT
    };
    V4L2Context *s = s1->priv_data;
    AVCodecContext *enc_ctx;
    uint32_t v4l2_pixfmt;
    if (s1->flags & AVFMT_FLAG_NONBLOCK)
        flags |= O_NONBLOCK;
    s->fd = open(s1->filename, flags);
    if (s->fd < 0) {
        res = AVERROR(errno);
        av_log(s1, AV_LOG_ERROR, "Unable to open V4L2 device '%s'\n", s1->filename);
        return res;
    }
    if (s1->nb_streams != 1 ||
        s1->streams[0]->codec->codec_type != AVMEDIA_TYPE_VIDEO ||
        s1->streams[0]->codec->codec_id   != AV_CODEC_ID_RAWVIDEO) {
        av_log(s1, AV_LOG_ERROR,
               "V4L2 output device supports only a single raw video stream\n");
        return AVERROR(EINVAL);
    }
    enc_ctx = s1->streams[0]->codec;
    v4l2_pixfmt = ff_fmt_ff2v4l(enc_ctx->pix_fmt, AV_CODEC_ID_RAWVIDEO);
    if (!v4l2_pixfmt) { 
        av_log(s1, AV_LOG_ERROR, "Unknown V4L2 pixel format equivalent for %s\n",
               av_get_pix_fmt_name(enc_ctx->pix_fmt));
        return AVERROR(EINVAL);
    }
    if (ioctl(s->fd, VIDIOC_G_FMT, &fmt) < 0) {
        res = AVERROR(errno);
        av_log(s1, AV_LOG_ERROR, "ioctl(VIDIOC_G_FMT): %s\n", av_err2str(res));
        return res;
    }
    fmt.fmt.pix.width       = enc_ctx->width;
    fmt.fmt.pix.height      = enc_ctx->height;
    fmt.fmt.pix.pixelformat = v4l2_pixfmt;
    fmt.fmt.pix.sizeimage   = av_image_get_buffer_size(enc_ctx->pix_fmt, enc_ctx->width, enc_ctx->height, 1);
    if (ioctl(s->fd, VIDIOC_S_FMT, &fmt) < 0) {
        res = AVERROR(errno);
        av_log(s1, AV_LOG_ERROR, "ioctl(VIDIOC_S_FMT): %s\n", av_err2str(res));
        return res;
    }
    return res;
}
