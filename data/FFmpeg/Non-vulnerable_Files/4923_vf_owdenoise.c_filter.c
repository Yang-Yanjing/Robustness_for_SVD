static void filter(OWDenoiseContext *s,
                   uint8_t       *dst, int dst_linesize,
                   const uint8_t *src, int src_linesize,
                   int width, int height, double strength)
{
    int x, y, i, j, depth = s->depth;
    while (1<<depth > width || 1<<depth > height)
        depth--;
    for (y = 0; y < height; y++)
        for(x = 0; x < width; x++)
            s->plane[0][0][y*s->linesize + x] = src[y*src_linesize + x];
    for (i = 0; i < depth; i++)
        decompose2D2(s->plane[i + 1], s->plane[i][0], s->plane[0] + 1, s->linesize, 1<<i, width, height);
    for (i = 0; i < depth; i++) {
        for (j = 1; j < 4; j++) {
            for (y = 0; y < height; y++) {
                for (x = 0; x < width; x++) {
                    double v = s->plane[i + 1][j][y*s->linesize + x];
                    if      (v >  strength) v -= strength;
                    else if (v < -strength) v += strength;
                    else                    v  = 0;
                    s->plane[i + 1][j][x + y*s->linesize] = v;
                }
            }
        }
    }
    for (i = depth-1; i >= 0; i--)
        compose2D2(s->plane[i][0], s->plane[i + 1], s->plane[0] + 1, s->linesize, 1<<i, width, height);
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            i = s->plane[0][0][y*s->linesize + x] + dither[x&7][y&7]*(1.0/64) + 1.0/128; 
            if ((unsigned)i > 255U) i = ~(i >> 31);
            dst[y*dst_linesize + x] = i;
        }
    }
}
