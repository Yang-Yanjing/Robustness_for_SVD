int av_write_frame(AVFormatContext *s, AVPacket *pkt)
{
    int ret;
    ret = check_packet(s, pkt);
    if (ret < 0)
        return ret;
    if (!pkt) {
        if (s->oformat->flags & AVFMT_ALLOW_FLUSH) {
            ret = s->oformat->write_packet(s, NULL);
            if (s->flush_packets && s->pb && s->pb->error >= 0 && s->flags & AVFMT_FLAG_FLUSH_PACKETS)
                avio_flush(s->pb);
            if (ret >= 0 && s->pb && s->pb->error < 0)
                ret = s->pb->error;
            return ret;
        }
        return 1;
    }
    ret = compute_pkt_fields2(s, s->streams[pkt->stream_index], pkt);
    if (ret < 0 && !(s->oformat->flags & AVFMT_NOTIMESTAMPS))
        return ret;
    ret = write_packet(s, pkt);
    if (ret >= 0 && s->pb && s->pb->error < 0)
        ret = s->pb->error;
    if (ret >= 0)
        s->streams[pkt->stream_index]->nb_frames++;
    return ret;
}
