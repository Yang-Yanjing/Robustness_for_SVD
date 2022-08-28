static inline void copy_backptr(LZOContext *c, int back, int cnt)
{
    register uint8_t *dst       = c->out;
    av_assert0(cnt > 0);
    if (dst - c->out_start < back) {
        c->error |= AV_LZO_INVALID_BACKPTR;
        return;
    }
    if (cnt > c->out_end - dst) {
        cnt       = FFMAX(c->out_end - dst, 0);
        c->error |= AV_LZO_OUTPUT_FULL;
    }
    av_memcpy_backptr(dst, back, cnt);
    c->out = dst + cnt;
}
