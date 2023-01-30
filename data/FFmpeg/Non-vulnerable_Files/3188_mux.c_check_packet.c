static int check_packet(AVFormatContext *s, AVPacket *pkt)
{
    if (!pkt)
        return 0;
    if (pkt->stream_index < 0 || pkt->stream_index >= s->nb_streams) {
        av_log(s, AV_LOG_ERROR, "Invalid packet stream index: %d\n",
               pkt->stream_index);
        return AVERROR(EINVAL);
    }
    if (s->streams[pkt->stream_index]->codec->codec_type == AVMEDIA_TYPE_ATTACHMENT) {
        av_log(s, AV_LOG_ERROR, "Received a packet for an attachment stream.\n");
        return AVERROR(EINVAL);
    }
    return 0;
}
