static int handle_server_bw(URLContext *s, RTMPPacket *pkt)
{
    RTMPContext *rt = s->priv_data;
    if (pkt->size < 4) {
        av_log(s, AV_LOG_ERROR,
               "Too short server bandwidth report packet (%d)\n",
               pkt->size);
        return AVERROR_INVALIDDATA;
    }
    rt->server_bw = AV_RB32(pkt->data);
    if (rt->server_bw <= 0) {
        av_log(s, AV_LOG_ERROR, "Incorrect server bandwidth %d\n",
               rt->server_bw);
        return AVERROR_INVALIDDATA;
    }
    av_log(s, AV_LOG_DEBUG, "Server bandwidth = %d\n", rt->server_bw);
    return 0;
}
