static AVFrame *downscale(AVFrame *in)
{
    int x, y;
    AVFrame *frame = av_frame_alloc();
    uint8_t *src, *dst;
    if (!frame)
        return NULL;
    frame->format = in->format;
    frame->width  = (in->width + 1) / 2;
    frame->height = (in->height+ 1) / 2;
    if (av_frame_get_buffer(frame, 32) < 0) {
        av_frame_free(&frame);
        return NULL;
    }
    src = in   ->data[0];
    dst = frame->data[0];
    for(y = 0; y < frame->height; y++) {
        for(x = 0; x < frame->width; x++) {
            dst[x] = (  src[2*x+0]
                      + src[2*x+1]
                      + src[2*x+0 + in->linesize[0]]
                      + src[2*x+1 + in->linesize[0]]
                      + 2) >> 2;
        }
        src += 2*in->linesize[0];
        dst += frame->linesize[0];
    }
    return frame;
}
