static int filter_frame(AVFilterLink *link, AVFrame *frame)
{
    FlipContext *flip = link->dst->priv;
    int i;
    for (i = 0; i < 4; i ++) {
        int vsub = i == 1 || i == 2 ? flip->vsub : 0;
        int height = FF_CEIL_RSHIFT(link->h, vsub);
        if (frame->data[i]) {
            frame->data[i] += (height - 1) * frame->linesize[i];
            frame->linesize[i] = -frame->linesize[i];
        }
    }
    return ff_filter_frame(link->dst->outputs[0], frame);
}
