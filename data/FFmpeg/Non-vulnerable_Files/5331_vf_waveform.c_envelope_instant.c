static void envelope_instant(WaveformContext *s, AVFrame *out, int plane, int component)
{
    const int dst_linesize = out->linesize[component];
    const uint8_t bg = s->bg_color[component];
    const int is_chroma = (component == 1 || component == 2);
    const int shift_w = (is_chroma ? s->desc->log2_chroma_w : 0);
    const int shift_h = (is_chroma ? s->desc->log2_chroma_h : 0);
    const int dst_h = FF_CEIL_RSHIFT(out->height, shift_h);
    const int dst_w = FF_CEIL_RSHIFT(out->width, shift_w);
    const int start = s->estart[plane];
    const int end = s->eend[plane];
    uint8_t *dst;
    int x, y;
    if (s->mode) {
        for (x = 0; x < dst_w; x++) {
            for (y = start; y < end; y++) {
                dst = out->data[component] + y * dst_linesize + x;
                if (dst[0] != bg) {
                    dst[0] = 255;
                    break;
                }
            }
            for (y = end - 1; y >= start; y--) {
                dst = out->data[component] + y * dst_linesize + x;
                if (dst[0] != bg) {
                    dst[0] = 255;
                    break;
                }
            }
        }
    } else {
        for (y = 0; y < dst_h; y++) {
            dst = out->data[component] + y * dst_linesize;
            for (x = start; x < end; x++) {
                if (dst[x] != bg) {
                    dst[x] = 255;
                    break;
                }
            }
            for (x = end - 1; x >= start; x--) {
                if (dst[x] != bg) {
                    dst[x] = 255;
                    break;
                }
            }
        }
    }
}
