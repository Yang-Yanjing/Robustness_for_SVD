static int caca_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    CACAContext *c = s->priv_data;
    caca_dither_bitmap(c->canvas, 0, 0, c->window_width, c->window_height, c->dither, pkt->data);
    caca_refresh_display(c->display);
    return 0;
}
