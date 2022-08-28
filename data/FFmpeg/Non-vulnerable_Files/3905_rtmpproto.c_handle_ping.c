static int handle_ping(URLContext *s, RTMPPacket *pkt)
{
    RTMPContext *rt = s->priv_data;
    int t, ret;
    if (pkt->size < 2) {
        av_log(s, AV_LOG_ERROR, "Too short ping packet (%d)\n",
               pkt->size);
        return AVERROR_INVALIDDATA;
    }
    t = AV_RB16(pkt->data);
    if (t == 6) {
        if ((ret = gen_pong(s, rt, pkt)) < 0)
            return ret;
    } else if (t == 26) {
        if (rt->swfsize) {
            if ((ret = gen_swf_verification(s, rt)) < 0)
                return ret;
        } else {
            av_log(s, AV_LOG_WARNING, "Ignoring SWFVerification request.\n");
        }
    }
    return 0;
}
