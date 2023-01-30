#define URAND_SEED (2463534242)
static uint32_t urand(uint32_t *seed)
{
    uint32_t s = *seed;
    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    return *seed = s;
}
