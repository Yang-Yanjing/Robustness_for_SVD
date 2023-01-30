static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    PullupContext *s = ctx->priv;
    PullupBuffer *b;
    PullupFrame *f;
    AVFrame *out;
    int p, ret = 0;
    b = pullup_get_buffer(s, 2);
    if (!b) {
        av_log(ctx, AV_LOG_WARNING, "Could not get buffer!\n");
        f = pullup_get_frame(s);
        pullup_release_frame(f);
        goto end;
    }
    av_image_copy(b->planes, s->planewidth,
                  (const uint8_t**)in->data, in->linesize,
                  inlink->format, inlink->w, inlink->h);
    p = in->interlaced_frame ? !in->top_field_first : 0;
    pullup_submit_field(s, b, p  );
    pullup_submit_field(s, b, p^1);
    if (in->repeat_pict)
        pullup_submit_field(s, b, p);
    pullup_release_buffer(b, 2);
    f = pullup_get_frame(s);
    if (!f)
        goto end;
    if (f->length < 2) {
        pullup_release_frame(f);
        f = pullup_get_frame(s);
        if (!f)
            goto end;
        if (f->length < 2) {
            pullup_release_frame(f);
            if (!in->repeat_pict)
                goto end;
            f = pullup_get_frame(s);
            if (!f)
                goto end;
            if (f->length < 2) {
                pullup_release_frame(f);
                goto end;
            }
        }
    }
    
    if (!f->buffer)
        pullup_pack_frame(s, f);
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    av_frame_copy_props(out, in);
    av_image_copy(out->data, out->linesize,
                  (const uint8_t**)f->buffer->planes, s->planewidth,
                  inlink->format, inlink->w, inlink->h);
    ret = ff_filter_frame(outlink, out);
    pullup_release_frame(f);
end:
    av_frame_free(&in);
    return ret;
}
