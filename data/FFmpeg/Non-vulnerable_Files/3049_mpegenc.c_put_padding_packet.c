static void put_padding_packet(AVFormatContext *ctx, AVIOContext *pb,
                               int packet_bytes)
{
    MpegMuxContext *s = ctx->priv_data;
    int i;
    avio_wb32(pb, PADDING_STREAM);
    avio_wb16(pb, packet_bytes - 6);
    if (!s->is_mpeg2) {
        avio_w8(pb, 0x0f);
        packet_bytes -= 7;
    } else
        packet_bytes -= 6;
    for (i = 0; i < packet_bytes; i++)
        avio_w8(pb, 0xff);
}
