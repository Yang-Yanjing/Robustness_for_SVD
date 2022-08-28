static void filter_line_c_16bit(void *dst1,
                                void *prev1, void *cur1, void *next1,
                                int w, int prefs, int mrefs, int parity,
                                int mode)
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
    FILTER(0, w, 1)
}
