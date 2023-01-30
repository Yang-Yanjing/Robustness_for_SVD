static double drand(void *opaque, double min, double max)
{
    return min + (max-min) / UINT_MAX * av_lfg_get(opaque);
}
