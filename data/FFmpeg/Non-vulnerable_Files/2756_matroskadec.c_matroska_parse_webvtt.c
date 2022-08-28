static int matroska_parse_webvtt(MatroskaDemuxContext *matroska,
                                 MatroskaTrack *track,
                                 AVStream *st,
                                 uint8_t *data, int data_len,
                                 uint64_t timecode,
                                 uint64_t duration,
                                 int64_t pos)
{
    AVPacket *pkt;
    uint8_t *id, *settings, *text, *buf;
    int id_len, settings_len, text_len;
    uint8_t *p, *q;
    int err;
    if (data_len <= 0)
        return AVERROR_INVALIDDATA;
    p = data;
    q = data + data_len;
    id = p;
    id_len = -1;
    while (p < q) {
        if (*p == '\r' || *p == '\n') {
            id_len = p - id;
            if (*p == '\r')
                p++;
            break;
        }
        p++;
    }
    if (p >= q || *p != '\n')
        return AVERROR_INVALIDDATA;
    p++;
    settings = p;
    settings_len = -1;
    while (p < q) {
        if (*p == '\r' || *p == '\n') {
            settings_len = p - settings;
            if (*p == '\r')
                p++;
            break;
        }
        p++;
    }
    if (p >= q || *p != '\n')
        return AVERROR_INVALIDDATA;
    p++;
    text = p;
    text_len = q - p;
    while (text_len > 0) {
        const int len = text_len - 1;
        const uint8_t c = p[len];
        if (c != '\r' && c != '\n')
            break;
        text_len = len;
    }
    if (text_len <= 0)
        return AVERROR_INVALIDDATA;
    pkt = av_mallocz(sizeof(*pkt));
    if (!pkt)
        return AVERROR(ENOMEM);
    err = av_new_packet(pkt, text_len);
    if (err < 0) {
        av_free(pkt);
        return AVERROR(err);
    }
    memcpy(pkt->data, text, text_len);
    if (id_len > 0) {
        buf = av_packet_new_side_data(pkt,
                                      AV_PKT_DATA_WEBVTT_IDENTIFIER,
                                      id_len);
        if (!buf) {
            av_free(pkt);
            return AVERROR(ENOMEM);
        }
        memcpy(buf, id, id_len);
    }
    if (settings_len > 0) {
        buf = av_packet_new_side_data(pkt,
                                      AV_PKT_DATA_WEBVTT_SETTINGS,
                                      settings_len);
        if (!buf) {
            av_free(pkt);
            return AVERROR(ENOMEM);
        }
        memcpy(buf, settings, settings_len);
    }
    
    
    pkt->stream_index = st->index;
    pkt->pts = timecode;
    
    
    pkt->duration = duration;
    pkt->pos = pos;
    dynarray_add(&matroska->packets, &matroska->num_packets, pkt);
    matroska->prev_pkt = pkt;
    return 0;
}
