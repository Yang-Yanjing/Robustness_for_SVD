static av_cold void uninit(AVFilterContext *ctx)
{
    int i;
    EBUR128Context *ebur128 = ctx->priv;
    av_log(ctx, AV_LOG_INFO, "Summary:\n\n"
           "  Integrated loudness:\n"
           "    I:         %5.1f LUFS\n"
           "    Threshold: %5.1f LUFS\n\n"
           "  Loudness range:\n"
           "    LRA:       %5.1f LU\n"
           "    Threshold: %5.1f LUFS\n"
           "    LRA low:   %5.1f LUFS\n"
           "    LRA high:  %5.1f LUFS",
           ebur128->integrated_loudness, ebur128->i400.rel_threshold,
           ebur128->loudness_range,      ebur128->i3000.rel_threshold,
           ebur128->lra_low, ebur128->lra_high);
#define PRINT_PEAK_SUMMARY(str, sp, ptype) do {                  \
    int ch;                                                      \
    double maxpeak;                                              \
    maxpeak = 0.0;                                               \
    if (ebur128->peak_mode & PEAK_MODE_ ## ptype ## _PEAKS) {    \
        for (ch = 0; ch < ebur128->nb_channels; ch++)            \
            maxpeak = FFMAX(maxpeak, sp[ch]);                    \
        av_log(ctx, AV_LOG_INFO, "\n\n  " str " peak:\n"         \
               "    Peak:      %5.1f dBFS",                      \
               DBFS(maxpeak));                                   \
    }                                                            \
} while (0)
    PRINT_PEAK_SUMMARY("Sample", ebur128->sample_peaks, SAMPLES);
    PRINT_PEAK_SUMMARY("True",   ebur128->true_peaks,   TRUE);
    av_log(ctx, AV_LOG_INFO, "\n");
    av_freep(&ebur128->y_line_ref);
    av_freep(&ebur128->ch_weighting);
    av_freep(&ebur128->true_peaks);
    av_freep(&ebur128->sample_peaks);
    av_freep(&ebur128->true_peaks_per_frame);
    av_freep(&ebur128->i400.histogram);
    av_freep(&ebur128->i3000.histogram);
    for (i = 0; i < ebur128->nb_channels; i++) {
        av_freep(&ebur128->i400.cache[i]);
        av_freep(&ebur128->i3000.cache[i]);
    }
    for (i = 0; i < ctx->nb_outputs; i++)
        av_freep(&ctx->output_pads[i].name);
    av_frame_free(&ebur128->outpicref);
#if CONFIG_SWRESAMPLE
    av_freep(&ebur128->swr_buf);
    swr_free(&ebur128->swr_ctx);
#endif
}
