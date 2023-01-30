static const double s_pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
static void init_gaussian_filter(DynamicAudioNormalizerContext *s)
{
    double total_weight = 0.0;
    const double sigma = (((s->filter_size / 2.0) - 1.0) / 3.0) + (1.0 / 3.0);
    double adjust;
    int i;
    
    const int offset = s->filter_size / 2;
    const double c1 = 1.0 / (sigma * sqrt(2.0 * s_pi));
    const double c2 = 2.0 * pow(sigma, 2.0);
    
    for (i = 0; i < s->filter_size; i++) {
        const int x = i - offset;
        s->weights[i] = c1 * exp(-(pow(x, 2.0) / c2));
        total_weight += s->weights[i];
    }
    
    adjust = 1.0 / total_weight;
    for (i = 0; i < s->filter_size; i++) {
        s->weights[i] *= adjust;
    }
}
