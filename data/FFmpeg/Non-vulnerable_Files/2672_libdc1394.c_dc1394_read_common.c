static inline int dc1394_read_common(AVFormatContext *c,
                                     struct dc1394_frame_format **select_fmt, struct dc1394_frame_rate **select_fps)
{
    dc1394_data* dc1394 = c->priv_data;
    AVStream* vst;
    struct dc1394_frame_format *fmt;
    struct dc1394_frame_rate *fps;
    enum AVPixelFormat pix_fmt;
    int width, height;
    AVRational framerate;
    int ret = 0;
    if ((pix_fmt = av_get_pix_fmt(dc1394->pixel_format)) == AV_PIX_FMT_NONE) {
        av_log(c, AV_LOG_ERROR, "No such pixel format: %s.\n", dc1394->pixel_format);
        ret = AVERROR(EINVAL);
        goto out;
    }
    if ((ret = av_parse_video_size(&width, &height, dc1394->video_size)) < 0) {
        av_log(c, AV_LOG_ERROR, "Could not parse video size '%s'.\n", dc1394->video_size);
        goto out;
    }
    if ((ret = av_parse_video_rate(&framerate, dc1394->framerate)) < 0) {
        av_log(c, AV_LOG_ERROR, "Could not parse framerate '%s'.\n", dc1394->framerate);
        goto out;
    }
    dc1394->frame_rate = av_rescale(1000, framerate.num, framerate.den);
    for (fmt = dc1394_frame_formats; fmt->width; fmt++)
         if (fmt->pix_fmt == pix_fmt && fmt->width == width && fmt->height == height)
             break;
    for (fps = dc1394_frame_rates; fps->frame_rate; fps++)
         if (fps->frame_rate == dc1394->frame_rate)
             break;
    if (!fps->frame_rate || !fmt->width) {
        av_log(c, AV_LOG_ERROR, "Can't find matching camera format for %s, %dx%d@%d:1000fps\n", av_get_pix_fmt_name(pix_fmt),
                                                                                                width, height, dc1394->frame_rate);
        ret = AVERROR(EINVAL);
        goto out;
    }
    
    vst = avformat_new_stream(c, NULL);
    if (!vst) {
        ret = AVERROR(ENOMEM);
        goto out;
    }
    avpriv_set_pts_info(vst, 64, 1, 1000);
    vst->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    vst->codec->codec_id = AV_CODEC_ID_RAWVIDEO;
    vst->codec->time_base.den = framerate.num;
    vst->codec->time_base.num = framerate.den;
    vst->codec->width = fmt->width;
    vst->codec->height = fmt->height;
    vst->codec->pix_fmt = fmt->pix_fmt;
    
    av_init_packet(&dc1394->packet);
    dc1394->packet.size = avpicture_get_size(fmt->pix_fmt, fmt->width, fmt->height);
    dc1394->packet.stream_index = vst->index;
    dc1394->packet.flags |= AV_PKT_FLAG_KEY;
    dc1394->current_frame = 0;
    vst->codec->bit_rate = av_rescale(dc1394->packet.size * 8, fps->frame_rate, 1000);
    *select_fps = fps;
    *select_fmt = fmt;
out:
    return ret;
}
