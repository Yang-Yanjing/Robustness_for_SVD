static int gdigrab_read_close(AVFormatContext *s1)
{
    struct gdigrab *s = s1->priv_data;
    if (s->show_region)
        gdigrab_region_wnd_destroy(s1, s);
    if (s->source_hdc)
        ReleaseDC(s->hwnd, s->source_hdc);
    if (s->dest_hdc)
        DeleteDC(s->dest_hdc);
    if (s->hbmp)
        DeleteObject(s->hbmp);
    if (s->source_hdc)
        DeleteDC(s->source_hdc);
    return 0;
}
