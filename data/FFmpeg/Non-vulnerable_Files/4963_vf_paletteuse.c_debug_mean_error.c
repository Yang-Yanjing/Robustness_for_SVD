static void debug_mean_error(PaletteUseContext *s, const AVFrame *in1,
                             const AVFrame *in2, int frame_count)
{
    int x, y;
    const uint32_t *palette = s->palette;
    uint32_t *src1 = (uint32_t *)in1->data[0];
    uint8_t  *src2 =             in2->data[0];
    const int src1_linesize = in1->linesize[0] >> 2;
    const int src2_linesize = in2->linesize[0];
    const float div = in1->width * in1->height * 3;
    unsigned mean_err = 0;
    for (y = 0; y < in1->height; y++) {
        for (x = 0; x < in1->width; x++) {
            const uint32_t c1 = src1[x];
            const uint32_t c2 = palette[src2[x]];
            const uint8_t rgb1[] = {c1 >> 16 & 0xff, c1 >> 8 & 0xff, c1 & 0xff};
            const uint8_t rgb2[] = {c2 >> 16 & 0xff, c2 >> 8 & 0xff, c2 & 0xff};
            mean_err += diff(rgb1, rgb2);
        }
        src1 += src1_linesize;
        src2 += src2_linesize;
    }
    s->total_mean_err += mean_err;
    av_log(NULL, AV_LOG_INFO, "MEP:%.3f TotalMEP:%.3f\n",
           mean_err / div, s->total_mean_err / (div * frame_count));
}
