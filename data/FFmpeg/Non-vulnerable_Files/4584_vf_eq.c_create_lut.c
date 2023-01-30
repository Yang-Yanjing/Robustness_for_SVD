static void create_lut(EQParameters *param)
{
    int i;
    double   g  = 1.0 / param->gamma;
    double   lw = 1.0 - param->gamma_weight;
    for (i = 0; i < 256; i++) {
        double v = i / 255.0;
        v = param->contrast * (v - 0.5) + 0.5 + param->brightness;
        if (v <= 0.0) {
            param->lut[i] = 0;
        } else {
            v = v * lw + pow(v, g) * param->gamma_weight;
            if (v >= 1.0)
                param->lut[i] = 255;
            else
                param->lut[i] = 256.0 * v;
        }
    }
    param->lut_clean = 1;
}
