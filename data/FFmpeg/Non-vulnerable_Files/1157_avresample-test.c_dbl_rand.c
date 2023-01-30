static double dbl_rand(AVLFG *lfg)
{
    return 2.0 * (av_lfg_get(lfg) / (double)UINT_MAX) - 1.0;
}
