static void decompose2D2(float *dst[4], float *src, float *temp[2],
                         int linesize, int step, int w, int h)
{
    decompose2D(temp[0], temp[1], src,     1, linesize, step, w, h);
    decompose2D( dst[0],  dst[1], temp[0], linesize, 1, step, h, w);
    decompose2D( dst[2],  dst[3], temp[1], linesize, 1, step, h, w);
}
