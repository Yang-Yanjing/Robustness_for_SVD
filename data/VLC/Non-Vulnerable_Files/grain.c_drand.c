/* Uniform random value between 0 and 1 */
static double drand(uint32_t *seed)
{
    return urand(seed) / (double)UINT32_MAX;
}
