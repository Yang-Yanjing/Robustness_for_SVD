static void apply_lut(HueContext *s,
                      uint8_t *udst, uint8_t *vdst, const int dst_linesize,
                      uint8_t *usrc, uint8_t *vsrc, const int src_linesize,
                      int w, int h)
{
    int i;
    while (h--) {
        for (i = 0; i < w; i++) {
            const int u = usrc[i];
            const int v = vsrc[i];
            udst[i] = s->lut_u[u][v];
            vdst[i] = s->lut_v[u][v];
        }
        usrc += src_linesize;
        vsrc += src_linesize;
        udst += dst_linesize;
        vdst += dst_linesize;
    }
}
}
static void apply_lut(HueContext *s,
                      uint8_t *udst, uint8_t *vdst, const int dst_linesize,
                      uint8_t *usrc, uint8_t *vsrc, const int src_linesize,
                      int w, int h)
{
    int i;
    while (h--) {
        for (i = 0; i < w; i++) {
            const int u = usrc[i];
            const int v = vsrc[i];
            udst[i] = s->lut_u[u][v];
            vdst[i] = s->lut_v[u][v];
        }
