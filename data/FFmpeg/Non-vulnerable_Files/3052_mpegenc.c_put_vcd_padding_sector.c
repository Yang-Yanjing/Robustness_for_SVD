static void put_vcd_padding_sector(AVFormatContext *ctx)
{
    




    MpegMuxContext *s = ctx->priv_data;
    int i;
    for (i = 0; i < s->packet_size; i++)
        avio_w8(ctx->pb, 0);
    s->vcd_padding_bytes_written += s->packet_size;
    avio_flush(ctx->pb);
    



    s->packet_number++;
}
