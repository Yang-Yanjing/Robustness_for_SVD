static void draw_frame(AVFilterContext *ctx,
                       AVFrame *main_buf,
                       AVFrame *alpha_buf)
{
    AlphaMergeContext *merge = ctx->priv;
    int h = main_buf->height;
    if (merge->is_packed_rgb) {
        int x, y;
        uint8_t *pin, *pout;
        for (y = 0; y < h; y++) {
            pin = alpha_buf->data[0] + y * alpha_buf->linesize[0];
            pout = main_buf->data[0] + y * main_buf->linesize[0] + merge->rgba_map[A];
            for (x = 0; x < main_buf->width; x++) {
                *pout = *pin;
                pin += 1;
                pout += 4;
            }
        }
    } else {
        int y;
        const int main_linesize = main_buf->linesize[A];
        const int alpha_linesize = alpha_buf->linesize[Y];
        for (y = 0; y < h && y < alpha_buf->height; y++) {
            memcpy(main_buf->data[A] + y * main_linesize,
                   alpha_buf->data[Y] + y * alpha_linesize,
                   FFMIN(main_linesize, alpha_linesize));
        }
    }
}
