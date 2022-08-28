static void solve_coefficients(double cm[3][3], double rgb[3][3], const double yuv[3][3])
{
    int i, j;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            cm[i][j] = yuv[i][0] * rgb[0][j] + yuv[i][1] * rgb[1][j] + yuv[i][2] * rgb[2][j];
}
