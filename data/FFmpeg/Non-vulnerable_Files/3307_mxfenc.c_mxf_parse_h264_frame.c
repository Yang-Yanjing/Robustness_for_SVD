static int mxf_parse_h264_frame(AVFormatContext *s, AVStream *st,
                                AVPacket *pkt, MXFIndexEntry *e)
{
    MXFContext *mxf = s->priv_data;
    MXFStreamContext *sc = st->priv_data;
    static const int mxf_h264_num_codec_uls = sizeof(mxf_h264_codec_uls) / sizeof(mxf_h264_codec_uls[0]);
    const uint8_t *buf = pkt->data;
    const uint8_t *buf_end = pkt->data + pkt->size;
    uint32_t state = -1;
    int extra_size = 512; 
    int i, frame_size;
    uint8_t uid_found;
    if (pkt->size > extra_size)
        buf_end -= pkt->size - extra_size; 
    for (;;) {
        buf = avpriv_find_start_code(buf, buf_end, &state);
        if (buf >= buf_end)
            break;
        --buf;
        switch (state & 0x1f) {
        case NAL_SPS:
            st->codec->profile = buf[1];
            e->flags |= 0x40;
            break;
        case NAL_PPS:
            if (e->flags & 0x40) { 
                e->flags |= 0x80; 
                extra_size = 0;
                buf = buf_end;
            }
            break;
        default:
            break;
        }
    }
    if (mxf->header_written)
        return 1;
    sc->aspect_ratio = (AVRational){ 16, 9 }; 
    sc->component_depth = 10; 
    sc->interlaced = st->codec->field_order != AV_FIELD_PROGRESSIVE ? 1 : 0;
    if (sc->interlaced)
        sc->field_dominance = 1; 
    uid_found = 0;
    frame_size = pkt->size + extra_size;
    for (i = 0; i < mxf_h264_num_codec_uls; i++) {
        if (frame_size == mxf_h264_codec_uls[i].frame_size && sc->interlaced == mxf_h264_codec_uls[i].interlaced) {
            sc->codec_ul = &mxf_h264_codec_uls[i].uid;
            return 1;
        } else if (st->codec->profile == mxf_h264_codec_uls[i].profile) {
            sc->codec_ul = &mxf_h264_codec_uls[i].uid;
            uid_found = 1;
        }
    }
    if (!uid_found) {
        av_log(s, AV_LOG_ERROR, "AVC Intra 50/100 supported only\n");
        return 0;
    }
    return 1;
}
