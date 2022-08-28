static int filter_frame(AVFilterLink *link, AVFrame *frame)
{
    AVFilterContext *avctx = link->dst;
    int res;
    if (res = av_frame_make_writable(frame))
        return res;
    if (res = avctx->internal->execute(avctx, do_colorkey_slice, frame, NULL, FFMIN(frame->height, avctx->graph->nb_threads)))
        return res;
    return ff_filter_frame(avctx->outputs[0], frame);
}
