static void load_palette(PaletteUseContext *s, const AVFrame *palette_frame)
{
    int i, x, y;
    const uint32_t *p = (const uint32_t *)palette_frame->data[0];
    const int p_linesize = palette_frame->linesize[0] >> 2;
    i = 0;
    for (y = 0; y < palette_frame->height; y++) {
        for (x = 0; x < palette_frame->width; x++)
            s->palette[i++] = p[x];
        p += p_linesize;
    }
    load_colormap(s);
    s->palette_loaded = 1;
}
