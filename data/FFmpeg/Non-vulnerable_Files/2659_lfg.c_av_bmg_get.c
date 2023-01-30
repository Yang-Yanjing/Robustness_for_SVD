void av_bmg_get(AVLFG *lfg, double out[2])
{
    double x1, x2, w;
    do {
        x1 = 2.0 / UINT_MAX * av_lfg_get(lfg) - 1.0;
        x2 = 2.0 / UINT_MAX * av_lfg_get(lfg) - 1.0;
        w  = x1 * x1 + x2 * x2;
    } while (w >= 1.0);
    w = sqrt((-2.0 * log(w)) / w);
    out[0] = x1 * w;
    out[1] = x2 * w;
}
