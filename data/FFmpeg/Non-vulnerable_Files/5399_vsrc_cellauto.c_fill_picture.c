static void fill_picture(AVFilterContext *ctx, AVFrame *picref)
{
    CellAutoContext *cellauto = ctx->priv;
    int i, j, k, row_idx = 0;
    uint8_t *p0 = picref->data[0];
    if (cellauto->scroll && cellauto->generation >= cellauto->h)
        
        row_idx = (cellauto->buf_row_idx + 1) % cellauto->h;
    
    for (i = 0; i < cellauto->h; i++) {
        uint8_t byte = 0;
        uint8_t *row = cellauto->buf + row_idx*cellauto->w;
        uint8_t *p = p0;
        for (k = 0, j = 0; j < cellauto->w; j++) {
            byte |= row[j]<<(7-k++);
            if (k==8 || j == cellauto->w-1) {
                k = 0;
                *p++ = byte;
                byte = 0;
            }
        }
        row_idx = (row_idx + 1) % cellauto->h;
        p0 += picref->linesize[0];
    }
}
