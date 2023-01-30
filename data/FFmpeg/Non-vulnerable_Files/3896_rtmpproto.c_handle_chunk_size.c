static int handle_chunk_size(URLContext *s, RTMPPacket *pkt)
{
    RTMPContext *rt = s->priv_data;
    int ret;
    if (pkt->size < 4) {
        av_log(s, AV_LOG_ERROR,
               "Too short chunk size change packet (%d)\n",
               pkt->size);
        return AVERROR_INVALIDDATA;
    }
    if (!rt->is_input) {
        

        if ((ret = ff_rtmp_packet_write(rt->stream, pkt, rt->out_chunk_size,
                                        &rt->prev_pkt[1], &rt->nb_prev_pkt[1])) < 0)
            return ret;
        rt->out_chunk_size = AV_RB32(pkt->data);
    }
    rt->in_chunk_size = AV_RB32(pkt->data);
    if (rt->in_chunk_size <= 0) {
        av_log(s, AV_LOG_ERROR, "Incorrect chunk size %d\n",
               rt->in_chunk_size);
        return AVERROR_INVALIDDATA;
    }
    av_log(s, AV_LOG_DEBUG, "New incoming chunk size = %d\n",
           rt->in_chunk_size);
    return 0;
}
