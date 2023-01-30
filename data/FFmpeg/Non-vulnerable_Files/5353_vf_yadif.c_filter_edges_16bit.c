static void filter_edges_16bit(void *dst1, void *prev1, void *cur1, void *next1,
                               int w, int prefs, int mrefs, int parity, int mode)
{
    uint16_t *dst  = dst1;
    uint16_t *prev = prev1;
    uint16_t *cur  = cur1;
    uint16_t *next = next1;
    int x;
    uint16_t *prev2 = parity ? prev : cur ;
    uint16_t *next2 = parity ? cur  : next;
    mrefs /= 2;
    prefs /= 2;
    FILTER(0, 3, 0)
    dst   = (uint16_t*)dst1  + w - (MAX_ALIGN/2-1);
    prev  = (uint16_t*)prev1 + w - (MAX_ALIGN/2-1);
    cur   = (uint16_t*)cur1  + w - (MAX_ALIGN/2-1);
    next  = (uint16_t*)next1 + w - (MAX_ALIGN/2-1);
    prev2 = (uint16_t*)(parity ? prev : cur);
    next2 = (uint16_t*)(parity ? cur  : next);
    FILTER(w - (MAX_ALIGN/2-1), w - 3, 1)
    FILTER(w - 3, w, 0)
}
