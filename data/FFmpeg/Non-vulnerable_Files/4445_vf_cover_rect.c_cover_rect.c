static void cover_rect(CoverContext *cover, AVFrame *in, int offx, int offy)
{
    int x, y, p;
    for (p = 0; p < 3; p++) {
        uint8_t *data = in->data[p] + (offx>>!!p) + (offy>>!!p) * in->linesize[p];
        const uint8_t *src = cover->cover_frame->data[p];
        int w = FF_CEIL_RSHIFT(cover->cover_frame->width , !!p);
        int h = FF_CEIL_RSHIFT(cover->cover_frame->height, !!p);
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                data[x] = src[x];
            }
            data += in->linesize[p];
            src += cover->cover_frame->linesize[p];
        }
    }
}
