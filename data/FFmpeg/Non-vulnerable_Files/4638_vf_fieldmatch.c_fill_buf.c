static void fill_buf(uint8_t *data, int w, int h, int linesize, uint8_t v)
{
    int y;
    for (y = 0; y < h; y++) {
        memset(data, v, w);
        data += linesize;
    }
}
