static void envelope_instant(VectorscopeContext *s, AVFrame *out)
{
    const int dlinesize = out->linesize[0];
    uint8_t *dpd = s->mode == COLOR || !s->is_yuv ? out->data[s->pd] : out->data[0];
    int i, j;
    for (i = 0; i < out->height; i++) {
        for (j = 0; j < out->width; j++) {
            const int pos = i * dlinesize + j;
            const int poa = (i - 1) * dlinesize + j;
            const int pob = (i + 1) * dlinesize + j;
            if (dpd[pos] && (((!j || !dpd[pos - 1]) || ((j == (out->width - 1)) || !dpd[pos + 1]))
                         || ((!i || !dpd[poa]) || ((i == (out->height - 1)) || !dpd[pob])))) {
                dpd[pos] = 255;
            }
        }
    }
}
