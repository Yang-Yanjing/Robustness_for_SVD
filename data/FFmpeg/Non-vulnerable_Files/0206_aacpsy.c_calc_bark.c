static av_cold float calc_bark(float f)
{
    return 13.3f * atanf(0.00076f * f) + 3.5f * atanf((f / 7500.0f) * (f / 7500.0f));
}
