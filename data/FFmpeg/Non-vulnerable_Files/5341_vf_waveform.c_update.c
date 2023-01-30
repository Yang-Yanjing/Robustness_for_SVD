static void update(uint8_t *target, int max, int intensity)
{
    if (*target <= max)
        *target += intensity;
    else
        *target = 255;
}
