static int caca_write_trailer(AVFormatContext *s)
{
    CACAContext *c = s->priv_data;
    av_freep(&c->window_title);
    if (c->display) {
        caca_free_display(c->display);
        c->display = NULL;
    }
    if (c->dither) {
        caca_free_dither(c->dither);
        c->dither = NULL;
    }
    if (c->canvas) {
        caca_free_canvas(c->canvas);
        c->canvas = NULL;
    }
    return 0;
}
