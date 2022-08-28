static void gen_waveform(HistogramContext *h, AVFrame *inpicref, AVFrame *outpicref,
                         int component, int intensity, int offset, int col_mode)
{
    const int plane = h->desc->comp[component].plane;
    const int mirror = h->waveform_mirror;
    const int is_chroma = (component == 1 || component == 2);
    const int shift_w = (is_chroma ? h->desc->log2_chroma_w : 0);
    const int shift_h = (is_chroma ? h->desc->log2_chroma_h : 0);
    const int src_linesize = inpicref->linesize[plane];
    const int dst_linesize = outpicref->linesize[plane];
    const int dst_signed_linesize = dst_linesize * (mirror == 1 ? -1 : 1);
    uint8_t *src_data = inpicref->data[plane];
    uint8_t *dst_data = outpicref->data[plane] + (col_mode ? (offset >> shift_h) * dst_linesize : offset >> shift_w);
    uint8_t * const dst_bottom_line = dst_data + dst_linesize * ((256 >> shift_h) - 1);
    uint8_t * const dst_line = (mirror ? dst_bottom_line : dst_data);
    const uint8_t max = 255 - intensity;
    const int src_h = FF_CEIL_RSHIFT(inpicref->height, shift_h);
    const int src_w = FF_CEIL_RSHIFT(inpicref->width, shift_w);
    uint8_t *dst, *p;
    int y;
    if (!col_mode && mirror)
        dst_data += 256 >> shift_w;
    for (y = 0; y < src_h; y++) {
        const uint8_t *src_data_end = src_data + src_w;
        dst = dst_line;
        for (p = src_data; p < src_data_end; p++) {
            uint8_t *target;
            if (col_mode) {
                target = dst++ + dst_signed_linesize * (*p >> shift_h);
            } else {
                if (mirror)
                    target = dst_data - (*p >> shift_w);
                else
                    target = dst_data + (*p >> shift_w);
            }
            if (*target <= max)
                *target += intensity;
            else
                *target = 255;
        }
        src_data += src_linesize;
        dst_data += dst_linesize;
    }
}
