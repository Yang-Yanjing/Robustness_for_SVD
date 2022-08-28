static uint64_t rand64(void) {
    uint64_t r = rand();
    r = (r << 32) | rand();
    return r;
}
