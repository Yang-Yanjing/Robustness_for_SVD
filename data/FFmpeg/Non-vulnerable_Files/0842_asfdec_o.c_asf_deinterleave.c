static int asf_deinterleave(AVFormatContext *s, ASFPacket *asf_pkt, int st_num)
{
    ASFContext *asf    = s->priv_data;
    ASFStream *asf_st  = asf->asf_st[st_num];
    unsigned char *p   = asf_pkt->avpkt.data;
    uint16_t pkt_len   = asf->asf_st[st_num]->virtual_pkt_len;
    uint16_t chunk_len = asf->asf_st[st_num]->virtual_chunk_len;
    int nchunks        = pkt_len / chunk_len;
    AVPacket pkt;
    int pos = 0, j, l, ret;
    if ((ret = av_new_packet(&pkt, asf_pkt->data_size)) < 0)
        return ret;
    while (asf_pkt->data_size >= asf_st->span * pkt_len + pos) {
        if (pos >= asf_pkt->data_size) {
            break;
        }
        for (l = 0; l < pkt_len; l++) {
            if (pos >= asf_pkt->data_size) {
                break;
            }
            for (j = 0; j < asf_st->span; j++) {
                if ((pos + chunk_len) >= asf_pkt->data_size)
                    break;
                memcpy(pkt.data + pos,
                       p + (j * nchunks + l) * chunk_len,
                       chunk_len);
                pos += chunk_len;
            }
        }
        p += asf_st->span * pkt_len;
        if (p > asf_pkt->avpkt.data + asf_pkt->data_size)
            break;
    }
    av_free_packet(&asf_pkt->avpkt);
    asf_pkt->avpkt = pkt;
    return 0;
}
