static int grab_read_header(AVFormatContext *s1)
{
    VideoData *s = s1->priv_data;
    AVStream *st;
    AVRational framerate;
    int ret = 0;
    if (!s->framerate)
        switch (s->standard) {
        case PAL:   s->framerate = av_strdup("pal");  break;
        case NTSC:  s->framerate = av_strdup("ntsc"); break;
        case SECAM: s->framerate = av_strdup("25");   break;
        default:
            av_log(s1, AV_LOG_ERROR, "Unknown standard.\n");
            ret = AVERROR(EINVAL);
            goto out;
        }
    if ((ret = av_parse_video_rate(&framerate, s->framerate)) < 0) {
        av_log(s1, AV_LOG_ERROR, "Could not parse framerate '%s'.\n", s->framerate);
        goto out;
    }
    st = avformat_new_stream(s1, NULL);
    if (!st) {
        ret = AVERROR(ENOMEM);
        goto out;
    }
    avpriv_set_pts_info(st, 64, 1, 1000000); 
    s->per_frame = ((uint64_t)1000000 * framerate.den) / framerate.num;
    st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    st->codec->pix_fmt = AV_PIX_FMT_YUV420P;
    st->codec->codec_id = AV_CODEC_ID_RAWVIDEO;
    st->codec->width = s->width;
    st->codec->height = s->height;
    st->codec->time_base.den = framerate.num;
    st->codec->time_base.num = framerate.den;
    if (bktr_init(s1->filename, s->width, s->height, s->standard,
                  &s->video_fd, &s->tuner_fd, -1, 0.0) < 0) {
        ret = AVERROR(EIO);
        goto out;
    }
    nsignals = 0;
    last_frame_time = 0;
out:
    return ret;
}
