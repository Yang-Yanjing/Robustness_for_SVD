static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    int i, j;
    AVFilterContext *ctx  = inlink->dst;
    ThumbContext *thumb   = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    int *hist = thumb->frames[thumb->n].histogram;
    const uint8_t *p = frame->data[0];
    
    thumb->frames[thumb->n].buf = frame;
    
    for (j = 0; j < inlink->h; j++) {
        for (i = 0; i < inlink->w; i++) {
            hist[0*256 + p[i*3    ]]++;
            hist[1*256 + p[i*3 + 1]]++;
            hist[2*256 + p[i*3 + 2]]++;
        }
        p += frame->linesize[0];
    }
    
    thumb->n++;
    if (thumb->n < thumb->n_frames)
        return 0;
    return ff_filter_frame(outlink, get_best_frame(ctx));
}
