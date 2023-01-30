static void mxf_write_d10_video_packet(AVFormatContext *s, AVStream *st, AVPacket *pkt)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    MXFStreamContext *sc = st->priv_data;
    int packet_size = (uint64_t)sc->video_bit_rate*mxf->time_base.num /
        (8*mxf->time_base.den); 
    int pad;
    packet_size += 16 + 4;
    packet_size += klv_fill_size(packet_size);
    klv_encode_ber4_length(pb, pkt->size);
    avio_write(pb, pkt->data, pkt->size);
    
    pad = packet_size - pkt->size - 16 - 4;
    if (pad > 20) {
        avio_write(s->pb, klv_fill_key, 16);
        pad -= 16 + 4;
        klv_encode_ber4_length(s->pb, pad);
        ffio_fill(s->pb, 0, pad);
        av_assert1(!(avio_tell(s->pb) & (KAG_SIZE-1)));
    } else {
        av_log(s, AV_LOG_WARNING, "cannot fill d-10 video packet\n");
        ffio_fill(s->pb, 0, pad);
    }
}
