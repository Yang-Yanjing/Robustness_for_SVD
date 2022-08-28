static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    FOCContext *foc = ctx->priv;
    float best_score;
    int best_x, best_y;
    int i;
    foc->haystack_frame[0] = av_frame_clone(in);
    for (i=1; i<foc->mipmaps; i++) {
        foc->haystack_frame[i] = downscale(foc->haystack_frame[i-1]);
    }
    best_score = search(foc, 0, 0,
                        FFMAX(foc->xmin, foc->last_x - 8),
                        FFMIN(foc->xmax, foc->last_x + 8),
                        FFMAX(foc->ymin, foc->last_y - 8),
                        FFMIN(foc->ymax, foc->last_y + 8),
                        &best_x, &best_y, 1.0);
    best_score = search(foc, 0, foc->mipmaps - 1, foc->xmin, foc->xmax, foc->ymin, foc->ymax,
                        &best_x, &best_y, best_score);
    for (i=0; i<MAX_MIPMAPS; i++) {
        av_frame_free(&foc->haystack_frame[i]);
    }
    if (best_score > foc->threshold) {
        return ff_filter_frame(ctx->outputs[0], in);
    }
    av_log(ctx, AV_LOG_DEBUG, "Found at %d %d score %f\n", best_x, best_y, best_score);
    foc->last_x = best_x;
    foc->last_y = best_y;
    av_frame_make_writable(in);
    av_dict_set_int(&in->metadata, "lavfi.rect.w", foc->obj_frame->width, 0);
    av_dict_set_int(&in->metadata, "lavfi.rect.h", foc->obj_frame->height, 0);
    av_dict_set_int(&in->metadata, "lavfi.rect.x", best_x, 0);
    av_dict_set_int(&in->metadata, "lavfi.rect.y", best_y, 0);
    return ff_filter_frame(ctx->outputs[0], in);
}
