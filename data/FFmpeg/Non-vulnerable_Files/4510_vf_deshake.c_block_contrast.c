static int block_contrast(uint8_t *src, int x, int y, int stride, int blocksize)
{
    int highest = 0;
    int lowest = 255;
    int i, j, pos;
    for (i = 0; i <= blocksize * 2; i++) {
        
        for (j = 0; j <= 15; j++) {
            pos = (y - i) * stride + (x - j);
            if (src[pos] < lowest)
                lowest = src[pos];
            else if (src[pos] > highest) {
                highest = src[pos];
            }
        }
    }
    return highest - lowest;
}
