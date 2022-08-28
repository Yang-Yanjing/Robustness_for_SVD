static double calc_stereo_rms(const float *samples, int nb_samples)
{
    int count = nb_samples;
    double sum = 1e-16;
    while (count--) {
        sum += samples[0] * samples[0] + samples[1] * samples[1];
        samples += 2;
    }
    return 10 * log10 (sum / nb_samples) + 90.0 - 3.0;
}
