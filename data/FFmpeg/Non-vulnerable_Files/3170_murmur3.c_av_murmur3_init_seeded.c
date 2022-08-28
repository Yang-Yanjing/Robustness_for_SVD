void av_murmur3_init_seeded(AVMurMur3 *c, uint64_t seed)
{
    memset(c, 0, sizeof(*c));
    c->h1 = c->h2 = seed;
}
