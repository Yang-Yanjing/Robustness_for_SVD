static int opengl_write_packet(AVFormatContext *h, AVPacket *pkt)
{
    return opengl_draw(h, pkt, 0, 1);
}
