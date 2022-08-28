static int write_packet(AVFormatContext *s1, AVPacket *pkt)
{
    const V4L2Context *s = s1->priv_data;
    if (write(s->fd, pkt->data, pkt->size) == -1)
        return AVERROR(errno);
    return 0;
}
