static CoefType calc_cpl_coord(CoefSumType energy_ch, CoefSumType energy_cpl)
{
    float coord = 0.125;
    if (energy_cpl > 0)
        coord *= sqrtf(energy_ch / energy_cpl);
    return FFMIN(coord, COEF_MAX);
}
