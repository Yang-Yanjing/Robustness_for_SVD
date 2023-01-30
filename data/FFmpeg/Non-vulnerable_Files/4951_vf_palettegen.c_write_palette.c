static void write_palette(AVFilterContext *ctx, AVFrame *out)
{
    const PaletteGenContext *s = ctx->priv;
    int x, y, box_id = 0;
    uint32_t *pal = (uint32_t *)out->data[0];
    const int pal_linesize = out->linesize[0] >> 2;
    uint32_t last_color = 0;
    for (y = 0; y < out->height; y++) {
        for (x = 0; x < out->width; x++) {
            if (box_id < s->nb_boxes) {
                pal[x] = s->boxes[box_id++].color;
                if ((x || y) && pal[x] == last_color)
                    av_log(ctx, AV_LOG_WARNING, "Dupped color: %08X\n", pal[x]);
                last_color = pal[x];
            } else {
                pal[x] = 0xff000000; 
            }
        }
        pal += pal_linesize;
    }
    if (s->reserve_transparent) {
        av_assert0(s->nb_boxes < 256);
        pal[out->width - pal_linesize - 1] = 0x0000ff00; 
    }
}
