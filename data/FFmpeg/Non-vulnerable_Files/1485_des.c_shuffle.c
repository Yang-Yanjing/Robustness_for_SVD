static uint64_t shuffle(uint64_t in, const uint8_t *shuffle, int shuffle_len) {
    int i;
    uint64_t res = 0;
    for (i = 0; i < shuffle_len; i++)
        res += res + ((in >> *shuffle++) & 1);
    return res;
}
