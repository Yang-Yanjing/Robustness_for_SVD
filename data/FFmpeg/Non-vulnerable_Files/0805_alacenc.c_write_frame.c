static int write_frame(AlacEncodeContext *s, AVPacket *avpkt,
                       uint8_t * const *samples)
{
    PutBitContext *pb = &s->pbctx;
    const enum AlacRawDataBlockType *ch_elements = ff_alac_channel_elements[s->avctx->channels - 1];
    const uint8_t *ch_map = ff_alac_channel_layout_offsets[s->avctx->channels - 1];
    int ch, element, sce, cpe;
    init_put_bits(pb, avpkt->data, avpkt->size);
    ch = element = sce = cpe = 0;
    while (ch < s->avctx->channels) {
        if (ch_elements[element] == TYPE_CPE) {
            write_element(s, TYPE_CPE, cpe, samples[ch_map[ch]],
                          samples[ch_map[ch + 1]]);
            cpe++;
            ch += 2;
        } else {
            write_element(s, TYPE_SCE, sce, samples[ch_map[ch]], NULL);
            sce++;
            ch++;
        }
        element++;
    }
    put_bits(pb, 3, TYPE_END);
    flush_put_bits(pb);
    return put_bits_count(pb) >> 3;
}
