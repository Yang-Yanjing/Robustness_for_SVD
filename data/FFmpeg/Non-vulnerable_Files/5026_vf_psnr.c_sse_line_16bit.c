static uint64_t sse_line_16bit(const uint8_t *_main_line, const uint8_t *_ref_line, int outw)
{
    int j;
    uint64_t m2 = 0;
    const uint16_t *main_line = (const uint16_t *) _main_line;
    const uint16_t *ref_line = (const uint16_t *) _ref_line;
    for (j = 0; j < outw; j++)
        m2 += pow2(main_line[j] - ref_line[j]);
    return m2;
}
