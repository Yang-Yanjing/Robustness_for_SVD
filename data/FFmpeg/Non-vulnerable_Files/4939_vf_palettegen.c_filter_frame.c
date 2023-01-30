static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    PaletteGenContext *s = ctx->priv;
    const int ret = s->prev_frame ? update_histogram_diff(s->histogram, s->prev_frame, in)
                                  : update_histogram_frame(s->histogram, in);
    if (ret > 0)
        s->nb_refs += ret;
    if (s->stats_mode == STATS_MODE_DIFF_FRAMES) {
        av_frame_free(&s->prev_frame);
        s->prev_frame = in;
    } else {
        av_frame_free(&in);
    }
    return ret;
}
