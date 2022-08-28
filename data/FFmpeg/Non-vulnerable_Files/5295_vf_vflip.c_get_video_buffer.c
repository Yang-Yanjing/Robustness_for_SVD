static AVFrame *get_video_buffer(AVFilterLink *link, int w, int h)
{
    FlipContext *flip = link->dst->priv;
    AVFrame *frame;
    int i;
    frame = ff_get_video_buffer(link->dst->outputs[0], w, h);
    if (!frame)
        return NULL;
    for (i = 0; i < 4; i ++) {
        int vsub = i == 1 || i == 2 ? flip->vsub : 0;
        int height = FF_CEIL_RSHIFT(h, vsub);
        if (frame->data[i]) {
            frame->data[i] += (height - 1) * frame->linesize[i];
            frame->linesize[i] = -frame->linesize[i];
        }
    }
    return frame;
}
