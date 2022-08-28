static void envelope_peak(WaveformContext *s, AVFrame *out, int plane, int component)
{
    const int dst_linesize = out->linesize[component];
    const int bg = s->bg_color[component];
    const int is_chroma = (component == 1 || component == 2);
    const int shift_w = (is_chroma ? s->desc->log2_chroma_w : 0);
    const int shift_h = (is_chroma ? s->desc->log2_chroma_h : 0);
    const int dst_h = FF_CEIL_RSHIFT(out->height, shift_h);
    const int dst_w = FF_CEIL_RSHIFT(out->width, shift_w);
    const int start = s->estart[plane];
    const int end = s->eend[plane];
    int *emax = s->emax[plane][component];
    int *emin = s->emin[plane][component];
    uint8_t *dst;
    int x, y;
    if (s->mode) {
        for (x = 0; x < dst_w; x++) {
            for (y = start; y < end && y < emin[x]; y++) {
                dst = out->data[component] + y * dst_linesize + x;
                if (dst[0] != bg) {
                    emin[x] = y;
                    break;
                }
            }
            for (y = end - 1; y >= start && y >= emax[x]; y--) {
                dst = out->data[component] + y * dst_linesize + x;
                if (dst[0] != bg) {
                    emax[x] = y;
                    break;
                }
            }
        }
        if (s->envelope == 3)
            envelope_instant(s, out, plane, component);
        for (x = 0; x < dst_w; x++) {
            dst = out->data[component] + emin[x] * dst_linesize + x;
            dst[0] = 255;
            dst = out->data[component] + emax[x] * dst_linesize + x;
            dst[0] = 255;
        }
    } else {
        for (y = 0; y < dst_h; y++) {
            dst = out->data[component] + y * dst_linesize;
            for (x = start; x < end && x < emin[y]; x++) {
                if (dst[x] != bg) {
                    emin[y] = x;
                    break;
                }
            }
            for (x = end - 1; x >= start && x >= emax[y]; x--) {
                if (dst[x] != bg) {
                    emax[y] = x;
                    break;
                }
            }
        }
        if (s->envelope == 3)
            envelope_instant(s, out, plane, component);
        for (y = 0; y < dst_h; y++) {
            dst = out->data[component] + y * dst_linesize + emin[y];
            dst[0] = 255;
            dst = out->data[component] + y * dst_linesize + emax[y];
            dst[0] = 255;
        }
    }
}
