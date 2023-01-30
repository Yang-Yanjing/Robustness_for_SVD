static void ac3_downmix_c_fixed16(int16_t **samples, int16_t (*matrix)[2],
                                  int out_ch, int in_ch, int len)
{
    int i, j;
    int v0, v1;
    if (out_ch == 2) {
        for (i = 0; i < len; i++) {
            v0 = v1 = 0;
            for (j = 0; j < in_ch; j++) {
                v0 += samples[j][i] * matrix[j][0];
                v1 += samples[j][i] * matrix[j][1];
            }
            samples[0][i] = (v0+2048)>>12;
            samples[1][i] = (v1+2048)>>12;
        }
    } else if (out_ch == 1) {
        for (i = 0; i < len; i++) {
            v0 = 0;
            for (j = 0; j < in_ch; j++)
                v0 += samples[j][i] * matrix[j][0];
            samples[0][i] = (v0+2048)>>12;
        }
    }
}
