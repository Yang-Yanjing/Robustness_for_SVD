static AVFrame *create_weave_frame(AVFilterContext *ctx, int match, int field,
                                   const AVFrame *prv, AVFrame *src, const AVFrame *nxt)
{
    AVFrame *dst;
    FieldMatchContext *fm = ctx->priv;
    if (match == mC) {
        dst = av_frame_clone(src);
    } else {
        AVFilterLink *outlink = ctx->outputs[0];
        dst = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!dst)
            return NULL;
        av_frame_copy_props(dst, src);
        switch (match) {
        case mP: copy_fields(fm, dst, src, 1-field); copy_fields(fm, dst, prv,   field); break;
        case mN: copy_fields(fm, dst, src, 1-field); copy_fields(fm, dst, nxt,   field); break;
        case mB: copy_fields(fm, dst, src,   field); copy_fields(fm, dst, prv, 1-field); break;
        case mU: copy_fields(fm, dst, src,   field); copy_fields(fm, dst, nxt, 1-field); break;
        default: av_assert0(0);
        }
    }
    return dst;
}
