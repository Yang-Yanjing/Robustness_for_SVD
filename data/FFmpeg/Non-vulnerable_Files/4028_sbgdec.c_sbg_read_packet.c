static int sbg_read_packet(AVFormatContext *avf, AVPacket *packet)
{
    int64_t ts, end_ts;
    ts = avf->streams[0]->cur_dts;
    end_ts = ts + avf->streams[0]->codec->frame_size;
    if (avf->streams[0]->duration != AV_NOPTS_VALUE)
        end_ts = FFMIN(avf->streams[0]->start_time + avf->streams[0]->duration,
                       end_ts);
    if (end_ts <= ts)
        return AVERROR_EOF;
    if (av_new_packet(packet, 12) < 0)
        return AVERROR(ENOMEM);
    packet->dts = packet->pts = ts;
    packet->duration = end_ts - ts;
    AV_WL64(packet->data + 0, ts);
    AV_WL32(packet->data + 8, packet->duration);
    return packet->size;
}
