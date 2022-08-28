static void ipdopd_reset(int8_t *ipd_hist, int8_t *opd_hist)
{
    int i;
    for (i = 0; i < PS_MAX_NR_IPDOPD; i++) {
        opd_hist[i] = 0;
        ipd_hist[i] = 0;
    }
}
