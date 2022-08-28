static void mxf_write_d10_audio_packet(AVFormatContext *s, AVStream *st, AVPacket *pkt)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    int frame_size = pkt->size / st->codec->block_align;
    uint8_t *samples = pkt->data;
    uint8_t *end = pkt->data + pkt->size;
    int i;
    klv_encode_ber4_length(pb, 4 + frame_size*4*8);
    avio_w8(pb, (frame_size == 1920 ? 0 : (mxf->edit_units_count-1) % 5 + 1));
    avio_wl16(pb, frame_size);
    avio_w8(pb, (1<<st->codec->channels)-1);
    while (samples < end) {
        for (i = 0; i < st->codec->channels; i++) {
            uint32_t sample;
            if (st->codec->codec_id == AV_CODEC_ID_PCM_S24LE) {
                sample = AV_RL24(samples)<< 4;
                samples += 3;
            } else {
                sample = AV_RL16(samples)<<12;
                samples += 2;
            }
            avio_wl32(pb, sample | i);
        }
        for (; i < 8; i++)
            avio_wl32(pb, i);
    }
}
