static uint32_t randomv = MKTAG('L','A','V','U');
static uint32_t av_get_random_seed_deterministic(void)
{
    return randomv = randomv * 1664525 + 1013904223;
}
