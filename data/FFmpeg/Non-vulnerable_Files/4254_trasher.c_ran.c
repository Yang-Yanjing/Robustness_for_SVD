static uint32_t state;
static uint32_t ran(void)
{
    return state = state * 1664525 + 1013904223;
}
