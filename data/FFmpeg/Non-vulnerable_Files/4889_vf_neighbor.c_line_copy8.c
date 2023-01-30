static inline void line_copy8(uint8_t *line, const uint8_t *srcp, int width, int mergin)
{
    int i;
    memcpy(line, srcp, width);
    for (i = mergin; i > 0; i--) {
        line[-i] = line[i];
        line[width - 1 + i] = line[width - 1 - i];
    }
}
