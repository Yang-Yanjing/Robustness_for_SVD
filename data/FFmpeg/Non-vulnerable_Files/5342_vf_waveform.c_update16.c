static void update16(uint16_t *target, int max, int intensity, int limit)
{
    if (*target <= max)
        *target += intensity;
    else
        *target = limit;
}
