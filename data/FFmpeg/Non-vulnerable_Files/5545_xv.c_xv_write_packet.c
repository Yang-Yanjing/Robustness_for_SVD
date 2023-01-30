static int xv_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    AVPicture pict;
    AVCodecContext *ctx = s->streams[0]->codec;
    avpicture_fill(&pict, pkt->data, ctx->pix_fmt, ctx->width, ctx->height);
    return write_picture(s, &pict);
}
