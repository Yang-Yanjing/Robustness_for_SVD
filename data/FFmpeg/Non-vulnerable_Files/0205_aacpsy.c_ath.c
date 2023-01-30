static av_cold float ath(float f, float add)
{
    f /= 1000.0f;
    return    3.64 * pow(f, -0.8)
            - 6.8  * exp(-0.6  * (f - 3.4) * (f - 3.4))
            + 6.0  * exp(-0.15 * (f - 8.7) * (f - 8.7))
            + (0.6 + 0.04 * add) * 0.001 * f * f * f * f;
}
