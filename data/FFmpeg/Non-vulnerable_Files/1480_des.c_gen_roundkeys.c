static void gen_roundkeys(uint64_t K[16], uint64_t key) {
    int i;
    
    uint64_t CDn = shuffle(key, PC1_shuffle, sizeof(PC1_shuffle));
    
    for (i = 0; i < 16; i++) {
        CDn = key_shift_left(CDn);
        if (i > 1 && i != 8 && i != 15)
            CDn = key_shift_left(CDn);
        K[i] = shuffle(CDn, PC2_shuffle, sizeof(PC2_shuffle));
    }
}
