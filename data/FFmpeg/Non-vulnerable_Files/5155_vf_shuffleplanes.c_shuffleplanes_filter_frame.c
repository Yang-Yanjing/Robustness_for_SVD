static int shuffleplanes_filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext          *ctx = inlink->dst;
    ShufflePlanesContext       *s = ctx->priv;
    uint8_t *shuffled_data[4]     = { NULL };
    int      shuffled_linesize[4] = { 0 };
    int i, ret;
    for (i = 0; i < s->planes; i++) {
        shuffled_data[i]     = frame->data[s->map[i]];
        shuffled_linesize[i] = frame->linesize[s->map[i]];
    }
    memcpy(frame->data,     shuffled_data,     sizeof(shuffled_data));
    memcpy(frame->linesize, shuffled_linesize, sizeof(shuffled_linesize));
    if (s->copy) {
        AVFrame *copy = ff_get_video_buffer(ctx->outputs[0], frame->width, frame->height);
        if (!copy) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        av_frame_copy(copy, frame);
        ret = av_frame_copy_props(copy, frame);
        if (ret < 0) {
            av_frame_free(&copy);
            goto fail;
        }
        av_frame_free(&frame);
        frame = copy;
    }
    return ff_filter_frame(ctx->outputs[0], frame);
fail:
    av_frame_free(&frame);
    return ret;
}
