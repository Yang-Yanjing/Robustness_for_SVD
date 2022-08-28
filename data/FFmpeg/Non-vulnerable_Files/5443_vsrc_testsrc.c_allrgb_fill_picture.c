AVFILTER_DEFINE_CLASS(allrgb);
static void allrgb_fill_picture(AVFilterContext *ctx, AVFrame *frame)
{
    unsigned x, y;
    const int linesize = frame->linesize[0];
    uint8_t *line = frame->data[0];
    for (y = 0; y < 4096; y++) {
        uint8_t *dst = line;
        for (x = 0; x < 4096; x++) {
            *dst++ = x;
            *dst++ = y;
            *dst++ = (x >> 8) | ((y >> 8) << 4);
        }
        line += linesize;
    }
}
