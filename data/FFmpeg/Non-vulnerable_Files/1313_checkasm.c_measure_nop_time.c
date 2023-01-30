static int measure_nop_time(void)
{
    uint16_t nops[10000];
    int i, nop_sum = 0;
    for (i = 0; i < 10000; i++) {
        uint64_t t = AV_READ_TIME();
        nops[i] = AV_READ_TIME() - t;
    }
    qsort(nops, 10000, sizeof(uint16_t), cmp_nop);
    for (i = 2500; i < 7500; i++)
        nop_sum += nops[i];
    return nop_sum / 500;
}
