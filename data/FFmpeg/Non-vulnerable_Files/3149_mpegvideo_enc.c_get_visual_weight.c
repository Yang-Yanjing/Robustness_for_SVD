static void get_visual_weight(int16_t *weight, uint8_t *ptr, int stride)
{
    int x, y;
    
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            int x2, y2;
            int sum = 0;
            int sqr = 0;
            int count = 0;
            for (y2 = FFMAX(y - 1, 0); y2 < FFMIN(8, y + 2); y2++) {
                for (x2= FFMAX(x - 1, 0); x2 < FFMIN(8, x + 2); x2++) {
                    int v = ptr[x2 + y2 * stride];
                    sum += v;
                    sqr += v * v;
                    count++;
                }
            }
            weight[x + 8 * y]= (36 * ff_sqrt(count * sqr - sum * sum)) / count;
        }
    }
}
