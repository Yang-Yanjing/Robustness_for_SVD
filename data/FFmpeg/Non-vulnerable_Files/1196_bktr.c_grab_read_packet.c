static int grab_read_packet(AVFormatContext *s1, AVPacket *pkt)
{
    VideoData *s = s1->priv_data;
    if (av_new_packet(pkt, video_buf_size) < 0)
        return AVERROR(EIO);
    bktr_getframe(s->per_frame);
    pkt->pts = av_gettime();
    memcpy(pkt->data, video_buf, video_buf_size);
    return video_buf_size;
}
