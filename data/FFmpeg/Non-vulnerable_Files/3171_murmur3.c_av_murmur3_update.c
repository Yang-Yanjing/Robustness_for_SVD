void av_murmur3_update(AVMurMur3 *c, const uint8_t *src, int len)
{
    const uint8_t *end;
    uint64_t h1 = c->h1, h2 = c->h2;
    uint64_t k1, k2;
    if (len <= 0) return;
    c->len += len;
    if (c->state_pos > 0) {
        while (c->state_pos < 16) {
            c->state[c->state_pos++] = *src++;
            if (--len <= 0) return;
        }
        c->state_pos = 0;
        k1 = get_k1(c->state);
        k2 = get_k2(c->state);
        h1 = update_h1(k1, h1, h2);
        h2 = update_h2(k2, h1, h2);
    }
    end = src + (len & ~15);
    while (src < end) {
        
        
        k1 = get_k1(src);
        k2 = get_k2(src);
        h1 = update_h1(k1, h1, h2);
        h2 = update_h2(k2, h1, h2);
        src += 16;
    }
    c->h1 = h1;
    c->h2 = h2;
    len &= 15;
    if (len > 0) {
        memcpy(c->state, src, len);
        c->state_pos = len;
    }
}
