AVFILTER_DEFINE_CLASS(allyuv);
static void allyuv_fill_picture(AVFilterContext *ctx, AVFrame *frame)
{
    const int ys = frame->linesize[0];
    const int us = frame->linesize[1];
    const int vs = frame->linesize[2];
    int x, y, j;
    for (y = 0; y < 4096; y++) {
        for (x = 0; x < 2048; x++) {
            frame->data[0][y * ys + x] = ((x / 8) % 256);
            frame->data[0][y * ys + 4095 - x] = ((x / 8) % 256);
        }
        for (x = 0; x < 2048; x+=8) {
            for (j = 0; j < 8; j++) {
                frame->data[1][vs * y + x + j]        = (y%16 + (j % 8) * 16);
                frame->data[1][vs * y + 4095 - x - j] = (128 + y%16 + (j % 8) * 16);
            }
        }
        for (x = 0; x < 4096; x++)
            frame->data[2][y * us + x] = 256 * y / 4096;
    }
}
