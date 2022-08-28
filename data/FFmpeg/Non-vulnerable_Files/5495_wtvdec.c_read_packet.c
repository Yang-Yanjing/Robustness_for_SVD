static int read_packet(AVFormatContext *s, AVPacket *pkt)
{
    WtvContext *wtv = s->priv_data;
    AVIOContext *pb = wtv->pb;
    int stream_index, len, ret;
    stream_index = parse_chunks(s, SEEK_TO_DATA, 0, &len);
    if (stream_index < 0)
        return stream_index;
    ret = av_get_packet(pb, pkt, len - 32);
    if (ret < 0)
        return ret;
    pkt->stream_index = stream_index;
    pkt->pts          = wtv->pts;
    avio_skip(pb, WTV_PAD8(len) - len);
    return 0;
}
