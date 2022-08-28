static int request_frame(AVFilterLink *outlink)
{
    TestSourceContext *test = outlink->src->priv;
    AVFrame *frame;
    if (test->duration >= 0 &&
        av_rescale_q(test->pts, test->time_base, AV_TIME_BASE_Q) >= test->duration)
        return AVERROR_EOF;
    if (test->draw_once) {
        if (test->draw_once_reset) {
            av_frame_free(&test->picref);
            test->draw_once_reset = 0;
        }
        if (!test->picref) {
            test->picref =
                ff_get_video_buffer(outlink, test->w, test->h);
            if (!test->picref)
                return AVERROR(ENOMEM);
            test->fill_picture_fn(outlink->src, test->picref);
        }
        frame = av_frame_clone(test->picref);
    } else
        frame = ff_get_video_buffer(outlink, test->w, test->h);
    if (!frame)
        return AVERROR(ENOMEM);
    frame->pts                 = test->pts;
    frame->key_frame           = 1;
    frame->interlaced_frame    = 0;
    frame->pict_type           = AV_PICTURE_TYPE_I;
    frame->sample_aspect_ratio = test->sar;
    if (!test->draw_once)
        test->fill_picture_fn(outlink->src, frame);
    test->pts++;
    test->nb_frame++;
    return ff_filter_frame(outlink, frame);
}
