static void ac3_downmix_c(float **samples, float (*matrix)[2],
                          int out_ch, int in_ch, int len)
{
    int i, j;
    float v0, v1;
    if (out_ch == 2) {
        for (i = 0; i < len; i++) {
            v0 = v1 = 0.0f;
            for (j = 0; j < in_ch; j++) {
                v0 += samples[j][i] * matrix[j][0];
                v1 += samples[j][i] * matrix[j][1];
            }
            samples[0][i] = v0;
            samples[1][i] = v1;
        }
    } else if (out_ch == 1) {
        for (i = 0; i < len; i++) {
            v0 = 0.0f;
            for (j = 0; j < in_ch; j++)
                v0 += samples[j][i] * matrix[j][0];
            samples[0][i] = v0;
        }
    }
}
