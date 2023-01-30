
static void build_abs_diff_mask(const uint8_t *prvp, int prv_linesize,
                                const uint8_t *nxtp, int nxt_linesize,
                                uint8_t *tbuffer,    int tbuf_linesize,
                                int width, int height)
{
    int y, x;
    prvp -= prv_linesize;
    nxtp -= nxt_linesize;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++)
            tbuffer[x] = FFABS(prvp[x] - nxtp[x]);
        prvp += prv_linesize;
        nxtp += nxt_linesize;
        tbuffer += tbuf_linesize;
    }
}
