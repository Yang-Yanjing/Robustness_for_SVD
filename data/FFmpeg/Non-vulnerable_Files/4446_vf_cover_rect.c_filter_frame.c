static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    CoverContext *cover = ctx->priv;
    AVDictionaryEntry *ex, *ey, *ew, *eh;
    int x = -1, y = -1, w = -1, h = -1;
    char *xendptr = NULL, *yendptr = NULL, *wendptr = NULL, *hendptr = NULL;
    ex = av_dict_get(in->metadata, "lavfi.rect.x", NULL, AV_DICT_MATCH_CASE);
    ey = av_dict_get(in->metadata, "lavfi.rect.y", NULL, AV_DICT_MATCH_CASE);
    ew = av_dict_get(in->metadata, "lavfi.rect.w", NULL, AV_DICT_MATCH_CASE);
    eh = av_dict_get(in->metadata, "lavfi.rect.h", NULL, AV_DICT_MATCH_CASE);
    if (ex && ey && ew && eh) {
        x = strtol(ex->value, &xendptr, 10);
        y = strtol(ey->value, &yendptr, 10);
        w = strtol(ew->value, &wendptr, 10);
        h = strtol(eh->value, &hendptr, 10);
    }
    if (!xendptr || *xendptr || !yendptr || *yendptr ||
        !wendptr || *wendptr || !hendptr || !hendptr
    ) {
        return ff_filter_frame(ctx->outputs[0], in);
    }
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }
    w = FFMIN(w, in->width  - x);
    h = FFMIN(h, in->height - y);
    if (w > in->width || h > in->height || w <= 0 || h <= 0)
        return AVERROR(EINVAL);
    if (cover->cover_frame) {
        if (w != cover->cover_frame->width || h != cover->cover_frame->height)
            return AVERROR(EINVAL);
    }
    cover->width  = w;
    cover->height = h;
    x = av_clip(x, 0, in->width  - w);
    y = av_clip(y, 0, in->height - h);
    av_frame_make_writable(in);
    if (cover->mode == MODE_BLUR) {
        blur (cover, in, x, y);
    } else {
        cover_rect(cover, in, x, y);
    }
    return ff_filter_frame(ctx->outputs[0], in);
}
