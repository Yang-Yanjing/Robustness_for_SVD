extern AVOutputFormat ff_mpeg2vob_muxer;
static int put_pack_header(AVFormatContext *ctx, uint8_t *buf,
                           int64_t timestamp)
{
    MpegMuxContext *s = ctx->priv_data;
    PutBitContext pb;
    init_put_bits(&pb, buf, 128);
    put_bits32(&pb, PACK_START_CODE);
    if (s->is_mpeg2)
        put_bits(&pb, 2, 0x1);
    else
        put_bits(&pb, 4, 0x2);
    put_bits(&pb,  3, (uint32_t)((timestamp >> 30) & 0x07));
    put_bits(&pb,  1, 1);
    put_bits(&pb, 15, (uint32_t)((timestamp >> 15) & 0x7fff));
    put_bits(&pb,  1, 1);
    put_bits(&pb, 15, (uint32_t)((timestamp)       & 0x7fff));
    put_bits(&pb,  1, 1);
    if (s->is_mpeg2)
        
        put_bits(&pb, 9, 0);
    put_bits(&pb, 1, 1);
    put_bits(&pb, 22, s->mux_rate);
    put_bits(&pb, 1, 1);
    if (s->is_mpeg2) {
        put_bits(&pb, 1, 1);
        put_bits(&pb, 5, 0x1f); 
        put_bits(&pb, 3, 0); 
    }
    flush_put_bits(&pb);
    return put_bits_ptr(&pb) - pb.buf;
}
