static void drawline(AVFrame *pic, int x, int y, int len, int step)
{
    int i;
    uint8_t *p = pic->data[0] + y*pic->linesize[0] + x*3;
    for (i = 0; i < len; i++) {
        memcpy(p, "\x00\xff\x00", 3);
        p += step;
    }
}
