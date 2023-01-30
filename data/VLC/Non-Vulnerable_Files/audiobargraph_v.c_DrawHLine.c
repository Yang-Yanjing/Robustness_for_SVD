static const uint8_t red[4] = { 37, 106, 191, 0xff };
static inline void DrawHLine(plane_t *p, int line, int col, const uint8_t color[4], int w)
{
    for (int j = 0; j < 4; j++)
        memset(&p[j].p_pixels[line * p[j].i_pitch + col], color[j], w);
}
