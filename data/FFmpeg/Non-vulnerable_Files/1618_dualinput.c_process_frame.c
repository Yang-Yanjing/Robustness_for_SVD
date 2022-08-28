static int process_frame(FFFrameSync *fs)
{
    AVFilterContext *ctx = fs->parent;
    FFDualInputContext *s = fs->opaque;
    AVFrame *mainpic = NULL, *secondpic = NULL;
    int ret = 0;
    if ((ret = ff_framesync_get_frame(&s->fs, 0, &mainpic,   1)) < 0 ||
        (ret = ff_framesync_get_frame(&s->fs, 1, &secondpic, 0)) < 0) {
        av_frame_free(&mainpic);
        return ret;
    }
    av_assert0(mainpic);
    mainpic->pts = av_rescale_q(mainpic->pts, s->fs.time_base, ctx->outputs[0]->time_base);
    if (secondpic && !ctx->is_disabled)
        mainpic = s->process(ctx, mainpic, secondpic);
    ret = ff_filter_frame(ctx->outputs[0], mainpic);
    av_assert1(ret != AVERROR(EAGAIN));
    return ret;
}
