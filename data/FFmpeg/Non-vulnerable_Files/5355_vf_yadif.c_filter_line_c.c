static void filter_line_c(void *dst1,
                          void *prev1, void *cur1, void *next1,
                          int w, int prefs, int mrefs, int parity, int mode)
{
    uint8_t *dst  = dst1;
    uint8_t *prev = prev1;
    uint8_t *cur  = cur1;
    uint8_t *next = next1;
    int x;
    uint8_t *prev2 = parity ? prev : cur ;
    uint8_t *next2 = parity ? cur  : next;
    



    FILTER(0, w, 1)
}
