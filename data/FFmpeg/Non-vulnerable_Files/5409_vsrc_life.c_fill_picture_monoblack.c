static void fill_picture_monoblack(AVFilterContext *ctx, AVFrame *picref)
{
    LifeContext *life = ctx->priv;
    uint8_t *buf = life->buf[life->buf_idx];
    int i, j, k;
    
    for (i = 0; i < life->h; i++) {
        uint8_t byte = 0;
        uint8_t *p = picref->data[0] + i * picref->linesize[0];
        for (k = 0, j = 0; j < life->w; j++) {
            byte |= (buf[i*life->w+j] == ALIVE_CELL)<<(7-k++);
            if (k==8 || j == life->w-1) {
                k = 0;
                *p++ = byte;
                byte = 0;
            }
        }
    }
}
