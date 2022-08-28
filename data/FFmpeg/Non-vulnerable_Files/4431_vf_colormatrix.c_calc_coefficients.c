static void calc_coefficients(AVFilterContext *ctx)
{
    ColorMatrixContext *color = ctx->priv;
    double rgb_coeffd[4][3][3];
    double yuv_convertd[16][3][3];
    int v = 0;
    int i, j, k;
    for (i = 0; i < 4; i++)
        inverse3x3(rgb_coeffd[i], yuv_coeff[i]);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            solve_coefficients(yuv_convertd[v], rgb_coeffd[i], yuv_coeff[j]);
            for (k = 0; k < 3; k++) {
                color->yuv_convert[v][k][0] = NS(yuv_convertd[v][k][0]);
                color->yuv_convert[v][k][1] = NS(yuv_convertd[v][k][1]);
                color->yuv_convert[v][k][2] = NS(yuv_convertd[v][k][2]);
            }
            if (color->yuv_convert[v][0][0] != 65536 || color->yuv_convert[v][1][0] != 0 ||
                color->yuv_convert[v][2][0] != 0) {
                av_log(ctx, AV_LOG_ERROR, "error calculating conversion coefficients\n");
            }
            v++;
        }
    }
}
