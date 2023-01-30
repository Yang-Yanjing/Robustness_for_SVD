static void filter_edges(void *dst1, void *prev1, void *cur1, void *next1,
                         int w, int prefs, int mrefs, int parity, int mode)
{
    uint8_t *dst  = dst1;
    uint8_t *prev = prev1;
    uint8_t *cur  = cur1;
    uint8_t *next = next1;
    int x;
    uint8_t *prev2 = parity ? prev : cur ;
    uint8_t *next2 = parity ? cur  : next;
    

    FILTER(0, 3, 0)
    dst  = (uint8_t*)dst1  + w - (MAX_ALIGN-1);
    prev = (uint8_t*)prev1 + w - (MAX_ALIGN-1);
    cur  = (uint8_t*)cur1  + w - (MAX_ALIGN-1);
    next = (uint8_t*)next1 + w - (MAX_ALIGN-1);
    prev2 = (uint8_t*)(parity ? prev : cur);
    next2 = (uint8_t*)(parity ? cur  : next);
    FILTER(w - (MAX_ALIGN-1), w - 3, 1)
    FILTER(w - 3, w, 0)
}
