static void apply_luma_lut(HueContext *s,
                           uint8_t *ldst, const int dst_linesize,
                           uint8_t *lsrc, const int src_linesize,
                           int w, int h)
{
    int i;
    while (h--) {
        for (i = 0; i < w; i++)
            ldst[i] = s->lut_l[lsrc[i]];
        lsrc += src_linesize;
        ldst += dst_linesize;
    }
}
