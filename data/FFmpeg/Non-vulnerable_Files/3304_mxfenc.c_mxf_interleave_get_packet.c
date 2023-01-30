static int mxf_interleave_get_packet(AVFormatContext *s, AVPacket *out, AVPacket *pkt, int flush)
{
    int i, stream_count = 0;
    for (i = 0; i < s->nb_streams; i++)
        stream_count += !!s->streams[i]->last_in_packet_buffer;
    if (stream_count && (s->nb_streams == stream_count || flush)) {
        AVPacketList *pktl = s->internal->packet_buffer;
        if (s->nb_streams != stream_count) {
            AVPacketList *last = NULL;
            
            while (pktl) {
                if (!stream_count || pktl->pkt.stream_index == 0)
                    break;
                last = pktl;
                pktl = pktl->next;
                stream_count--;
            }
            
            while (pktl) {
                AVPacketList *next = pktl->next;
                if(s->streams[pktl->pkt.stream_index]->last_in_packet_buffer == pktl)
                    s->streams[pktl->pkt.stream_index]->last_in_packet_buffer= NULL;
                av_free_packet(&pktl->pkt);
                av_freep(&pktl);
                pktl = next;
            }
            if (last)
                last->next = NULL;
            else {
                s->internal->packet_buffer = NULL;
                s->internal->packet_buffer_end= NULL;
                goto out;
            }
            pktl = s->internal->packet_buffer;
        }
        *out = pktl->pkt;
        av_log(s, AV_LOG_TRACE, "out st:%d dts:%"PRId64"\n", (*out).stream_index, (*out).dts);
        s->internal->packet_buffer = pktl->next;
        if(s->streams[pktl->pkt.stream_index]->last_in_packet_buffer == pktl)
            s->streams[pktl->pkt.stream_index]->last_in_packet_buffer= NULL;
        if(!s->internal->packet_buffer)
            s->internal->packet_buffer_end= NULL;
        av_freep(&pktl);
        return 1;
    } else {
    out:
        av_init_packet(out);
        return 0;
    }
}
