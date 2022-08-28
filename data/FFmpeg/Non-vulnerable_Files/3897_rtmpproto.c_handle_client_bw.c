static int handle_client_bw(URLContext *s, RTMPPacket *pkt)
{
    RTMPContext *rt = s->priv_data;
    if (pkt->size < 4) {
        av_log(s, AV_LOG_ERROR,
               "Client bandwidth report packet is less than 4 bytes long (%d)\n",
               pkt->size);
        return AVERROR_INVALIDDATA;
    }
    rt->client_report_size = AV_RB32(pkt->data);
    if (rt->client_report_size <= 0) {
        av_log(s, AV_LOG_ERROR, "Incorrect client bandwidth %d\n",
                rt->client_report_size);
        return AVERROR_INVALIDDATA;
    }
    av_log(s, AV_LOG_DEBUG, "Client bandwidth = %d\n", rt->client_report_size);
    rt->client_report_size >>= 1;
    return 0;
}
