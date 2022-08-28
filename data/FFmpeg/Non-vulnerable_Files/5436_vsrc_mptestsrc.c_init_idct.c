static double c[64];
static void init_idct(void)
{
    int i, j;
    for (i = 0; i < 8; i++) {
        double s = i == 0 ? sqrt(0.125) : 0.5;
        for (j = 0; j < 8; j++)
            c[i*8+j] = s*cos((M_PI/8.0)*i*(j+0.5));
    }
}
