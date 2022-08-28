static int handle_invoke(URLContext *s, RTMPPacket *pkt)
{
    RTMPContext *rt = s->priv_data;
    int ret = 0;
    
    if (ff_amf_match_string(pkt->data, pkt->size, "_error")) {
        if ((ret = handle_invoke_error(s, pkt)) < 0)
            return ret;
    } else if (ff_amf_match_string(pkt->data, pkt->size, "_result")) {
        if ((ret = handle_invoke_result(s, pkt)) < 0)
            return ret;
    } else if (ff_amf_match_string(pkt->data, pkt->size, "onStatus")) {
        if ((ret = handle_invoke_status(s, pkt)) < 0)
            return ret;
    } else if (ff_amf_match_string(pkt->data, pkt->size, "onBWDone")) {
        if ((ret = gen_check_bw(s, rt)) < 0)
            return ret;
    } else if (ff_amf_match_string(pkt->data, pkt->size, "releaseStream") ||
               ff_amf_match_string(pkt->data, pkt->size, "FCPublish")     ||
               ff_amf_match_string(pkt->data, pkt->size, "publish")       ||
               ff_amf_match_string(pkt->data, pkt->size, "play")          ||
               ff_amf_match_string(pkt->data, pkt->size, "_checkbw")      ||
               ff_amf_match_string(pkt->data, pkt->size, "createStream")) {
        if ((ret = send_invoke_response(s, pkt)) < 0)
            return ret;
    }
    return ret;
}
