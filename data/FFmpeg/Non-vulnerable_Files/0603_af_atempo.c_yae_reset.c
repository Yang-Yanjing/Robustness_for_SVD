static int yae_reset(ATempoContext *atempo,
                     enum AVSampleFormat format,
                     int sample_rate,
                     int channels)
{
    const int sample_size = av_get_bytes_per_sample(format);
    uint32_t nlevels  = 0;
    uint32_t pot;
    int i;
    atempo->format   = format;
    atempo->channels = channels;
    atempo->stride   = sample_size * channels;
    
    atempo->window = sample_rate / 24;
    
    nlevels = av_log2(atempo->window);
    pot = 1 << nlevels;
    av_assert0(pot <= atempo->window);
    if (pot < atempo->window) {
        atempo->window = pot * 2;
        nlevels++;
    }
    
    RE_MALLOC_OR_FAIL(atempo->frag[0].data, atempo->window * atempo->stride);
    RE_MALLOC_OR_FAIL(atempo->frag[1].data, atempo->window * atempo->stride);
    RE_MALLOC_OR_FAIL(atempo->frag[0].xdat, atempo->window * sizeof(FFTComplex));
    RE_MALLOC_OR_FAIL(atempo->frag[1].xdat, atempo->window * sizeof(FFTComplex));
    
    av_rdft_end(atempo->real_to_complex);
    atempo->real_to_complex = NULL;
    av_rdft_end(atempo->complex_to_real);
    atempo->complex_to_real = NULL;
    atempo->real_to_complex = av_rdft_init(nlevels + 1, DFT_R2C);
    if (!atempo->real_to_complex) {
        yae_release_buffers(atempo);
        return AVERROR(ENOMEM);
    }
    atempo->complex_to_real = av_rdft_init(nlevels + 1, IDFT_C2R);
    if (!atempo->complex_to_real) {
        yae_release_buffers(atempo);
        return AVERROR(ENOMEM);
    }
    RE_MALLOC_OR_FAIL(atempo->correlation, atempo->window * sizeof(FFTComplex));
    atempo->ring = atempo->window * 3;
    RE_MALLOC_OR_FAIL(atempo->buffer, atempo->ring * atempo->stride);
    
    RE_MALLOC_OR_FAIL(atempo->hann, atempo->window * sizeof(float));
    for (i = 0; i < atempo->window; i++) {
        double t = (double)i / (double)(atempo->window - 1);
        double h = 0.5 * (1.0 - cos(2.0 * M_PI * t));
        atempo->hann[i] = (float)h;
    }
    yae_clear(atempo);
    return 0;
}
