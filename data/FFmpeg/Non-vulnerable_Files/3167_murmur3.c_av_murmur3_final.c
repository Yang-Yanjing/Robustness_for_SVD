void av_murmur3_final(AVMurMur3 *c, uint8_t dst[16])
{
    uint64_t h1 = c->h1, h2 = c->h2;
    memset(c->state + c->state_pos, 0, sizeof(c->state) - c->state_pos);
    h1 ^= get_k1(c->state) ^ c->len;
    h2 ^= get_k2(c->state) ^ c->len;
    h1 += h2;
    h2 += h1;
    h1 = fmix(h1);
    h2 = fmix(h2);
    h1 += h2;
    h2 += h1;
    AV_WL64(dst, h1);
    AV_WL64(dst + 8, h2);
}
