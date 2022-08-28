static int do_colorkey_slice(AVFilterContext *avctx, void *arg, int jobnr, int nb_jobs)
{
    AVFrame *frame = arg;
    const int slice_start = (frame->height * jobnr) / nb_jobs;
    const int slice_end = (frame->height * (jobnr + 1)) / nb_jobs;
    ColorkeyContext *ctx = avctx->priv;
    int o, x, y;
    for (y = slice_start; y < slice_end; ++y) {
        for (x = 0; x < frame->width; ++x) {
            o = frame->linesize[0] * y + x * 4;
            frame->data[0][o + ctx->co[3]] =
                do_colorkey_pixel(ctx,
                                  frame->data[0][o + ctx->co[0]],
                                  frame->data[0][o + ctx->co[1]],
                                  frame->data[0][o + ctx->co[2]]);
        }
    }
    return 0;
}
