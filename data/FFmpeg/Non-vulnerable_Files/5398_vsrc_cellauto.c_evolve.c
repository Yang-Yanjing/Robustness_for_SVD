static void evolve(AVFilterContext *ctx)
{
    CellAutoContext *cellauto = ctx->priv;
    int i, v, pos[3];
    uint8_t *row, *prev_row = cellauto->buf + cellauto->buf_row_idx * cellauto->w;
    enum { NW, N, NE };
    cellauto->buf_prev_row_idx = cellauto->buf_row_idx;
    cellauto->buf_row_idx      = cellauto->buf_row_idx == cellauto->h-1 ? 0 : cellauto->buf_row_idx+1;
    row = cellauto->buf + cellauto->w * cellauto->buf_row_idx;
    for (i = 0; i < cellauto->w; i++) {
        if (cellauto->stitch) {
            pos[NW] = i-1 < 0 ? cellauto->w-1 : i-1;
            pos[N]  = i;
            pos[NE] = i+1 == cellauto->w ? 0  : i+1;
            v = prev_row[pos[NW]]<<2 | prev_row[pos[N]]<<1 | prev_row[pos[NE]];
        } else {
            v = 0;
            v|= i-1 >= 0          ? prev_row[i-1]<<2 : 0;
            v|=                     prev_row[i  ]<<1    ;
            v|= i+1 < cellauto->w ? prev_row[i+1]    : 0;
        }
        row[i] = !!(cellauto->rule & (1<<v));
        ff_dlog(ctx, "i:%d context:%c%c%c -> cell:%d\n", i,
                v&4?'@':' ', v&2?'@':' ', v&1?'@':' ', row[i]);
    }
    cellauto->generation++;
}
