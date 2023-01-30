static float calc_reduced_thr_3gpp(AacPsyBand *band, float min_snr,
                                   float reduction)
{
    float thr = band->thr;
    if (band->energy > thr) {
        thr = sqrtf(thr);
        thr = sqrtf(thr) + reduction;
        thr *= thr;
        thr *= thr;
        




        if (thr > band->energy * min_snr && band->avoid_holes != PSY_3GPP_AH_NONE) {
            thr = FFMAX(band->thr, band->energy * min_snr);
            band->avoid_holes = PSY_3GPP_AH_ACTIVE;
        }
    }
    return thr;
}
