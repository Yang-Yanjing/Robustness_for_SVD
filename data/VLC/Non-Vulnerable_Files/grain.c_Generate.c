 */
static int Generate(int16_t *bank, int h_min, int h_max, int v_min, int v_max)
{
    const int N = BANK_SIZE;
    double *workspace = calloc(3 * N * N, sizeof(*workspace));
    if (!workspace)
        return VLC_ENOMEM;
    double *gn        = &workspace[0 * N * N];
    double *cij       = &workspace[1 * N * N];
    double *tmp       = &workspace[2 * N * N];
    /* Create a gaussian noise matrix */
    assert((N % 2) == 0);
    uint32_t seed = URAND_SEED;
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N/2; x++) {
            grand(&gn[y * N + 2 * x + 0], &gn[y * N + 2 * x + 1], &seed);
        }
    }
    /* Clear non selected frequency.
     * Only the central band is kept */
    int zero = 0;
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            if ((x < h_min && y < v_min) || x > h_max || y > v_max) {
                gn[y * N + x] = 0.0;
                zero++;
            }
        }
    }
    const double correction = sqrt((double)N * N  / (N * N - zero));
    /* Filter the gaussian noise using an IDCT
     * The algo is simple/stupid and does C * GN * Ct */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cij[i * N + j] = i == 0 ? sqrt(1.0f / N) :
                                      sqrt(2.0f / N) * cos((2 * j + 1) * i * M_PI / 2 / N);
        }
    }
    //mtime_t tmul_0 = mdate();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double v = 0.0;
            for (int k = 0; k < N; k++)
                v += gn[i * N + k] * cij[k * N + j];
            tmp[i * N + j] = v;
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double v = 0.0;
            for (int k = 0; k < N; k++)
                v += cij[k * N + i] * tmp[k * N + j];
            /* Do not bias when rounding */
            int vq;
            if (v >= 0)
                vq =  (int)( v * correction * 127 + 0.5);
            else
                vq = -(int)(-v * correction * 127 + 0.5);
            bank[i * N + j] = VLC_CLIP(vq, INT16_MIN, INT16_MAX);
        }
    }
    //mtime_t mul_duration = mdate() - tmul_0;
    //fprintf(stderr, "IDCT took %d ms\n", (int)(mul_duration / 1000));
    free(workspace);
    return VLC_SUCCESS;
}
