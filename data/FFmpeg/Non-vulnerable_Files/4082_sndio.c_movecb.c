static inline void movecb(void *addr, int delta)
{
    SndioData *s = addr;
    s->hwpos += delta * s->channels * s->bps;
}
