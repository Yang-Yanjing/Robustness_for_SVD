static void print_stats(AVFilterContext *ctx)
{
    VolDetectContext *vd = ctx->priv;
    int i, max_volume, shift;
    uint64_t nb_samples = 0, power = 0, nb_samples_shift = 0, sum = 0;
    uint64_t histdb[MAX_DB + 1] = { 0 };
    for (i = 0; i < 0x10000; i++)
        nb_samples += vd->histogram[i];
    av_log(ctx, AV_LOG_INFO, "n_samples: %"PRId64"\n", nb_samples);
    if (!nb_samples)
        return;
    



    shift = av_log2(nb_samples >> 33);
    for (i = 0; i < 0x10000; i++) {
        nb_samples_shift += vd->histogram[i] >> shift;
        power += (i - 0x8000) * (i - 0x8000) * (vd->histogram[i] >> shift);
    }
    if (!nb_samples_shift)
        return;
    power = (power + nb_samples_shift / 2) / nb_samples_shift;
    av_assert0(power <= 0x8000 * 0x8000);
    av_log(ctx, AV_LOG_INFO, "mean_volume: %.1f dB\n", -logdb(power));
    max_volume = 0x8000;
    while (max_volume > 0 && !vd->histogram[0x8000 + max_volume] &&
                             !vd->histogram[0x8000 - max_volume])
        max_volume--;
    av_log(ctx, AV_LOG_INFO, "max_volume: %.1f dB\n", -logdb(max_volume * max_volume));
    for (i = 0; i < 0x10000; i++)
        histdb[(int)logdb((i - 0x8000) * (i - 0x8000))] += vd->histogram[i];
    for (i = 0; i <= MAX_DB && !histdb[i]; i++);
    for (; i <= MAX_DB && sum < nb_samples / 1000; i++) {
        av_log(ctx, AV_LOG_INFO, "histogram_%ddb: %"PRId64"\n", i, histdb[i]);
        sum += histdb[i];
    }
}
