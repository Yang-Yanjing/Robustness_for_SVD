static uint32_t f_func(uint32_t r, uint64_t k) {
    int i;
    uint32_t out = 0;
    
    r = (r << 1) | (r >> 31);
    
    for (i = 7; i >= 0; i--) {
        uint8_t tmp = (r ^ k) & 0x3f;
#if CONFIG_SMALL
        uint8_t v = S_boxes[i][tmp >> 1];
        if (tmp & 1) v >>= 4;
        out = (out >> 4) | (v << 28);
#else
        out |= S_boxes_P_shuffle[i][tmp];
#endif
        
        r = (r >> 4) | (r << 28);
        k >>= 6;
    }
#if CONFIG_SMALL
    out = shuffle(out, P_shuffle, sizeof(P_shuffle));
#endif
    return out;
}
